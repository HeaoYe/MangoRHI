#include "vulkan_context.hpp"

namespace MangoRHI {
    VulkanContext *vulkan_context;

    VulkanContext::VulkanContext() {
        device = std::make_unique<VulkanDevice>();
        swapchain = std::make_unique<VulkanSwapchain>();
        render_pass = std::make_unique<VulkanRenderPass>();
        framebuffer = std::make_unique<VulkanFrameBuffer>();
        descriptor_pool = std::make_unique<VulkanDescriptorPool>();
        synchronization = std::make_unique<VulkanSynchronization>();
        command_pool = std::make_unique<VulkanCommandPool>();
        resource_factory = std::make_unique<VulkanResourceFactory>();
        render_pass->create_render_target(MANGORHI_SURFACE_RENDER_TARGET_NAME, RenderTargetUsage::eColor);
    }

    void VulkanContext::set_api_info(const void *info) {
        this->info = static_cast<const VulkanContextInfo *>(info);
    }

    void VulkanContext::set_device_name(const char *name) {
        this->device->set_name(name);
    }

    void VulkanContext::set_swapchain_image_count(u32 count) {
        this->swapchain->set_image_count(count);
    }

    void VulkanContext::set_clear_value(const char *render_target_name, ClearValue value) {
        render_pass->get_render_targets()[render_pass->get_render_target_index_by_name(render_target_name)]->set_clear_value(value);
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
        #if defined (MANGORHI_DEBUG)
        RHI_INFO("MangoRHI Debug Mode")
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

        device->create();
        depth_format = device->get_supported_depth_format();
        max_multisample_count = device->get_max_multisample_count();
        if (multisample_count > max_multisample_count) {
            RHI_ERROR("Multisample count({}) more then max multisample count({}).", multisample_count, max_multisample_count)
            return Result::eFailed;
        }
        swapchain->create();
        command_pool->create();
        resource_factory->create();
        descriptor_pool->create();
        render_pass->create();
        framebuffer->create();
        synchronization->create();
        for (u32 index = 0; index < max_in_flight_frame_count; index++) {
            auto &command = commands.emplace_back(new VulkanCommand());
            command_pool->allocate(CommandLevel::ePrimary, *command);
        }
        return Result::eSuccess;
    }

    Result VulkanContext::destroy() {
        component_destroy()

        VK_CHECK(vkDeviceWaitIdle(device->get_logical_device()))

        descriptor_pool->destroy();
        for (auto &command : commands) {
            command_pool->free(*command);
        }
        commands.clear();
        synchronization->destroy();
        framebuffer->destroy();
        render_pass->destroy();
        resource_factory->destroy();
        command_pool->destroy();
        swapchain->destroy();
        device->destroy();

        RHI_DEBUG("Destroy vulkan surface -> 0x{:x}", (AddrType)surface)
        vkDestroySurfaceKHR(instance, surface, allocator);

        RHI_DEBUG("Destroy vulkan instance -> 0x{:x}", (AddrType)instance)
        vkDestroyInstance(instance, allocator);

        return Result::eSuccess;
    }

    void VulkanContext::recreate_resources() {
        VK_CHECK(vkDeviceWaitIdle(device->get_logical_device()))

        swapchain->recreate();
        render_pass->recreate_render_targets();
        framebuffer->recreate();

        for (auto &descriptor_set : g_vulkan_descriptor_sets) {
            for (auto &descriptor : descriptor_set->get_descriptors()) {
                if (descriptor->get_type() == DescriptorType::eInputRenderTarget) {
                    descriptor->update(*descriptor_set);
                }
            }
        }
    }

    Result VulkanContext::begin_frame() {
        Result res;
        if ((res = swapchain->acquire_next_frame()) != Result::eSuccess) {
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

        if ((res = render_pass->begin_render_pass(command)) != Result::eSuccess) {
            RHI_ERROR("Begin render pass error {}", to_string(res));
            return res;
        }

        return Result::eSuccess;
    }

    Result VulkanContext::end_frame() {
        Result res;
        auto &command = get_current_command_reference();

        if ((res = render_pass->end_render_pass((VulkanCommand &)command)) != Result::eSuccess) {
            RHI_ERROR("End render pass error {}", to_string(res));
            return res;
        }

        if ((res = command.end_render()) != Result::eSuccess) {
            RHI_ERROR("End command buffer error {}", to_string(res));
            return res;
        }

        if ((res = swapchain->present()) != Result::eSuccess) {
            if (res == Result::eNeedToRecreate) {
                recreate_resources();
                return Result::eRecreating;
            }
            return res;
        }

        current_in_flight_frame_index = (current_in_flight_frame_index + 1) % max_in_flight_frame_count;

        return Result::eSuccess;
    }

    VkFormat VulkanContext::find_supported_format(const STL_IMPL::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
        for (const auto &format : candidates) {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(device->get_physical_device(), format, &properties);
            if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        RHI_ERROR("Failed to find supported vulkan format")
        return VK_FORMAT_UNDEFINED;
    }

    VkImageView VulkanContext::create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect, uint32_t mipmap_levels) const {
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
            .levelCount = mipmap_levels,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };
        VK_CHECK(vkCreateImageView(device->get_logical_device(), &image_view_create_info, allocator, &image_view))
        return image_view;
    }

    u32 VulkanContext::find_memory_index(u32 type_filter, VkMemoryPropertyFlags flags) const {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(device->get_physical_device(), &properties);

        for (u32 i = 0; i < properties.memoryTypeCount; i++) {
            if (type_filter & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags) {
                return i;
            }
        }
        RHI_ERROR("Cannot find suitable memory type")
        return -1;
    }

    void VulkanContext::transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, u32 mipmap_levels) const {
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
        } else {
            RHI_ERROR("Unsupported layout transition");
        }

        VulkanCommand command;
        command_pool->allocate_single_use(command);
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
            .levelCount = mipmap_levels,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };
        if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        vkCmdPipelineBarrier(command.get_command_buffer(), src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        command_pool->free(command);
    }
}
