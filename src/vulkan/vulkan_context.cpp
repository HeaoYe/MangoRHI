#include "vulkan_context.hpp"

namespace MangoRHI {
    VulkanContext *vulkan_context;

    void VulkanContext::set_api_info(const void *info) {
        this->info = (const VulkanContextInfo *)info;
    }

    void VulkanContext::set_device_name(const char *name) {
        this->device.set_name(name);
    }

    void VulkanContext::set_swapchain_image_count(u32 count) {
        this->swapchain.set_image_count(count);
    }

    void VulkanContext::set_max_in_flight_frame_count(u32 count) {
        this->max_in_flight_frame_count = count;
    }

    void VulkanContext::set_clear_color(ColorClearValue clear_color) {
        this->swapchain.get_render_target().set_clear_color(ClearValue { .color = clear_color });
    }

    RenderTarget *VulkanContext::create_render_target() {
        auto *render_target = new VulkanRenderTarget();
        render_targets.push_back(render_target);
        return render_target;
    }

    Shader *VulkanContext::create_shader(const char *filename) {
        auto *shader = new VulkanShader();
        shaders.push_back(shader);
        shader->set_filename(filename);
        return shader;
    }

    VertexBuffer *VulkanContext::create_vertex_buffer() {
        auto *vertex_buffer = new VulkanVertexBuffer();
        vertex_buffer->set_count(1024);
        vertex_buffers.push_back(vertex_buffer);
        return vertex_buffer;
    }

    IndexBuffer *VulkanContext::create_index_buffer() {
        auto *index_buffer = new VulkanIndexBuffer();
        index_buffer->set_count(1024);
        index_buffers.push_back(index_buffer);
        return index_buffer;
    }

    Sampler *VulkanContext::create_sampler() {
        auto *sampler = new VulkanSampler();
        samplers.push_back(sampler);
        return sampler;
    }

    Texture *VulkanContext::create_texture() {
        auto *texture = new VulkanTexture();
        textures.push_back(texture);
        return texture;
    }

    void VulkanContext::resize(u32 width, u32 height) {
        // No Impl For Vulkan
    }

    Result VulkanContext::create() {
        component_create()

        VkApplicationInfo app_info { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
        app_info.pApplicationName = info->app_name;
        app_info.pEngineName = info->engine_name;
        app_info.apiVersion = VK_API_VERSION_1_3;
        VkInstanceCreateInfo instance_create_info { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instance_create_info.pApplicationInfo = &app_info;
        instance_create_info.ppEnabledExtensionNames = info->extensions;
        instance_create_info.enabledExtensionCount = info->extension_count;
        #if defined (MANGO_DEBUG)
        const char *layers[] = { "VK_LAYER_KHRONOS_validation" };
        instance_create_info.ppEnabledLayerNames = layers;
        instance_create_info.enabledLayerCount = 1;
        #else
        instance_create_info.enabledLayerCount = 0;
        #endif
        VK_CHECK(vkCreateInstance(&instance_create_info, allocator, &instance))
        RHI_DEBUG("Create vulkan instance -> 0x{:x}", (AddrType)instance)

        surface = info->surface_create_callback(instance, allocator);
        if (surface == VK_NULL_HANDLE) {
            RHI_ERROR("Cannot create vulkan surface")
            return Result::eFailed;
        }
        RHI_DEBUG("Create vulkan surface -> 0x{:x}", (AddrType)surface)

        device.create();
        swapchain.create();
        command_pool.create();
        for (auto &render_target : render_targets) {
            render_target->create();
        }
        for (auto &shader : shaders) {
            shader->create();
        }
        for (auto &sampler : samplers) {
            sampler->create();
        }
        for (auto &texture : textures) {
            texture->create();
        }
        descriptor_pool.create();
        render_pass.create();
        framebuffer.create();
        synchronization.create();
        for (u32 index = 0; index < max_in_flight_frame_count; index++) {
            auto *command = new VulkanCommand();
            commands.push_back(command);
            command_pool.allocate(CommandLevel::ePrimary, command);
        }
        for (auto &vertex_buffer : vertex_buffers) {
            vertex_buffer->create();
        }
        for (auto &index_buffer : index_buffers) {
            index_buffer->create();
        }

        return Result::eSuccess;
    }

    Result VulkanContext::destroy() {
        component_destroy()

        VK_CHECK(vkDeviceWaitIdle(device.get_logical_device()))

        descriptor_pool.destroy();
        for (auto &vertex_buffer : vertex_buffers) {
            vertex_buffer->destroy();
        }
        for (auto &index_buffer : index_buffers) {
            index_buffer->destroy();
        }
        for (auto &command : commands) {
            command_pool.free(command);
        }
        synchronization.destroy();
        framebuffer.destroy();
        render_pass.destroy();
        for (auto &texture : textures) {
            texture->destroy();
        }
        for (auto &sampler : samplers) {
            sampler->destroy();
        }
        for (auto &shader : shaders) {
            shader->destroy();
        }
        for (auto &render_target : render_targets) {
            render_target->destroy();
        }
        command_pool.destroy();
        swapchain.destroy();
        device.destroy();

        RHI_DEBUG("Destroy vulkan surface -> 0x{:x}", (AddrType)surface)
        vkDestroySurfaceKHR(instance, surface, allocator);

        RHI_DEBUG("Destroy vulkan instance -> 0x{:x}", (AddrType)instance)
        vkDestroyInstance(instance, allocator);

        return Result::eSuccess;
    }

    void VulkanContext::recreate_resources() {
        VK_CHECK(vkDeviceWaitIdle(device.get_logical_device()))
        swapchain.recreate();
        for (auto &render_target : render_targets) {
            render_target->recreate();
        }
        framebuffer.recreate();
    }

    Result VulkanContext::begin_frame() {
        Result res;
        if ((res = swapchain.acquire_next_frame()) != Result::eSuccess) {
            if (res == Result::eNeedToRecreate) {
                recreate_resources();
                return Result::eRecreating;
            }
            return res;
        }

        auto &command = (VulkanCommand &)get_current_command_reference();
        if ((res = command.begin_render()) != Result::eSuccess) {
            RHI_ERROR("Begin command buffer error {}", to_string(res));
            return res;
        }

        if ((res = render_pass.begin_render_pass((VulkanCommand *)&command)) != Result::eSuccess) {
            RHI_ERROR("Begin render pass error {}", to_string(res));
            return res;
        }
        
        return Result::eSuccess;
    }

    Result VulkanContext::end_frame() {
        Result res;
        auto &command = get_current_command_reference();

        if ((res = render_pass.end_render_pass((VulkanCommand *)&command)) != Result::eSuccess) {
            RHI_ERROR("End render pass error {}", to_string(res));
            return res;
        }

        if ((res = command.end_render()) != Result::eSuccess) {
            RHI_ERROR("End command buffer error {}", to_string(res));
            return res;
        }

        if ((res = swapchain.present()) != Result::eSuccess) {
            if (res == Result::eNeedToRecreate) {
                recreate_resources();
                return Result::eRecreating;
            }
            return res;
        }

        current_in_flight_frame_index = (current_in_flight_frame_index + 1) % max_in_flight_frame_count;

        return Result::eSuccess;
    }

    VkImageView VulkanContext::create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect) const {
        VkImageView image_view;
        VkImageViewCreateInfo image_view_create_info { .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        image_view_create_info.image = image;
        image_view_create_info.format = format;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.components = VkComponentMapping {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A,
        };
        image_view_create_info.subresourceRange = VkImageSubresourceRange {
            .aspectMask = aspect,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };
        VK_CHECK(vkCreateImageView(device.get_logical_device(), &image_view_create_info, allocator, &image_view))
        return image_view;
    }

    u32 VulkanContext::find_memory_index(u32 type_filter, VkMemoryPropertyFlags flags) const {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(device.get_physical_device(), &properties);

        for (u32 i = 0; i < properties.memoryTypeCount; i++) {
            if (type_filter & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags) {
                return i;
            }
        }
        RHI_ERROR("Cannot find suitable memory type")
        return -1;
    }

    void VulkanContext::transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) const {
        VkAccessFlags src_access = 0, dst_access = 0;
        VkPipelineStageFlags src_stage = 0, dst_stage = 0;
        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            src_access = 0;
            dst_stage = VK_ACCESS_TRANSFER_WRITE_BIT;

            src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            src_access = VK_ACCESS_TRANSFER_WRITE_BIT;
            dst_stage = VK_ACCESS_SHADER_READ_BIT;

            src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            src_access = 0;
            dst_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            RHI_ERROR("Unsupported layout transition");
        }

        VulkanCommand command;
        command_pool.allocate_single_use(&command);
        VkImageMemoryBarrier barrier { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcAccessMask = src_access;
        barrier.dstAccessMask = dst_access;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange = VkImageSubresourceRange {
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };
        if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            // if (hasStencilComponent(format)) {
            //     barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            // }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        vkCmdPipelineBarrier(command.get_command_buffer(), src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        command_pool.free(&command);
    }
}
