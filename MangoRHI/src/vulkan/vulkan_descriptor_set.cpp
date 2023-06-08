#include "vulkan_descriptor_set.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    STL_IMPL::unordered_map<VkDescriptorType, u32> g_descriptor_info = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0 }
    };
    STL_IMPL::vector<VulkanDescriptorSet *> g_vulkan_descriptor_sets(0);

    Result VulkanUniformDescriptor::create() {
        component_create()

        u32 align = 0x40;
        while (align < size) {
            align *= 2;
        }
        size = align;

        in_flight_buffers.resize(vulkan_context->get_max_in_flight_frame_count());
        mapped_pointers.resize(vulkan_context->get_max_in_flight_frame_count());
        for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
            in_flight_buffers[in_flight_index] = new VulkanBuffer();
            in_flight_buffers[in_flight_index]->set_size(size * get_count());
            in_flight_buffers[in_flight_index]->set_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            in_flight_buffers[in_flight_index]->set_properties(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            in_flight_buffers[in_flight_index]->create();
            mapped_pointers[in_flight_index] = in_flight_buffers[in_flight_index]->map();
        }

        return Result::eSuccess;
    }

    Result VulkanUniformDescriptor::destroy() {
        component_destroy()

        for (auto &buffer : in_flight_buffers) {
            buffer->unmap();
            buffer->destroy();
            delete buffer;
        }
        mapped_pointers.clear();
        in_flight_buffers.clear();

        return Result::eSuccess;
    }

    void VulkanUniformDescriptor::update(VulkanDescriptorSet *descriptor_set) {
        STL_IMPL::vector<VkDescriptorBufferInfo> descriptor_buffer_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            descriptor_buffer_infos[index].offset = index * size;
            descriptor_buffer_infos[index].range = size;
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pBufferInfo = descriptor_buffer_infos.data();
        write_descriptor_set.descriptorType = get_binding().descriptorType;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstBinding = get_binding_index();
        write_descriptor_set.dstArrayElement = 0;
        for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
            for (u32 index = 0; index < get_count(); index++) {
                descriptor_buffer_infos[index].buffer = in_flight_buffers[in_flight_index]->get_buffer();
            }
            write_descriptor_set.dstSet = descriptor_set->get_in_flight_descriptor_sets()[in_flight_index];
            vkUpdateDescriptorSets(vulkan_context->get_device().get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
        }
    }

    void *VulkanUniformDescriptor::get_current_mapped_pointer() const {
        return mapped_pointers[vulkan_context->get_current_in_flight_frame_index()];
    }

    void VulkanTextureDescriptor::add_texture(VulkanTexture *textures) {
        this->textures.push_back(textures);
    }

    void VulkanTextureDescriptor::set_texture(u32 index, VulkanTexture *texture) {
        this->textures[index] = texture;
    }

    Result VulkanTextureDescriptor::create() {
        component_create()

        return Result::eSuccess;
    }

    Result VulkanTextureDescriptor::destroy() {
        component_destroy()

        return Result::eSuccess;
    }

    void VulkanTextureDescriptor::update(VulkanDescriptorSet *descriptor_set) {
        STL_IMPL::vector<VkDescriptorImageInfo> descriptor_image_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            descriptor_image_infos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptor_image_infos[index].imageView = textures[index]->get_image().get_image_view();
            descriptor_image_infos[index].sampler = textures[index]->get_sampler().get_sampler();
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pImageInfo = descriptor_image_infos.data();
        write_descriptor_set.descriptorType = get_binding().descriptorType;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstBinding = get_binding_index();
        write_descriptor_set.dstArrayElement = 0;
        for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
            write_descriptor_set.dstSet = descriptor_set->get_in_flight_descriptor_sets()[in_flight_index];
            vkUpdateDescriptorSets(vulkan_context->get_device().get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
        }
    }

    Result VulkanInputRenderTargetDescriptor::create() {
        component_create()

        return Result::eSuccess;
    }

    Result VulkanInputRenderTargetDescriptor::destroy() {
        component_destroy()

        return Result::eSuccess;
    }

    void VulkanInputRenderTargetDescriptor::add_render_target(VulkanRenderTarget *render_target, VulkanSampler *sampler) {
        render_targets.push_back(render_target);
        samplers.push_back(sampler);
    }

    void VulkanInputRenderTargetDescriptor::set_render_target(u32 index, VulkanRenderTarget *render_target, VulkanSampler *sampler) {
        render_targets[index] = render_target;
        samplers[index] = sampler;
    }

    void VulkanInputRenderTargetDescriptor::update(VulkanDescriptorSet *descriptor_set) {
        STL_IMPL::vector<VkDescriptorImageInfo> descriptor_image_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            descriptor_image_infos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            if (render_targets[index]->get_is_each_frame_render_target() == MG_TRUE) {
                descriptor_image_infos[index].imageView = render_targets[index]->get_image_views()[vulkan_context->get_swapchain().get_image_index()];
            } else {
                descriptor_image_infos[index].imageView = render_targets[index]->get_image_views()[0];
            }
            descriptor_image_infos[index].sampler = samplers[index]->get_sampler();
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pImageInfo = descriptor_image_infos.data();
        write_descriptor_set.descriptorType = get_binding().descriptorType;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstBinding = get_binding_index();
        write_descriptor_set.dstArrayElement = 0;
        for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
            write_descriptor_set.dstSet = descriptor_set->get_in_flight_descriptor_sets()[in_flight_index];
            vkUpdateDescriptorSets(vulkan_context->get_device().get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
        }
    }

    u32 VulkanDescriptorSet::add_uniforms(DescriptorStage stage, u32 size, u32 count) {
        auto *uniform_descriptor = new VulkanUniformDescriptor();
        descriptors.push_back(uniform_descriptor);
        uniform_descriptor->set_size(size);
        setup_descriptor_binding(uniform_descriptor, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, stage, count);
        return _current_binding - 1;
    }

    u32 VulkanDescriptorSet::add_textures(DescriptorStage stage, Texture **textures, u32 count) {
        auto *texture_descriptor = new VulkanTextureDescriptor();
        for (u32 index = 0; index < count; index++) {
            texture_descriptor->add_texture((VulkanTexture *)textures[index]);
        }
        descriptors.push_back(texture_descriptor);
        setup_descriptor_binding(texture_descriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage, count);
        return _current_binding - 1;
    }

    u32 VulkanDescriptorSet::add_input_render_targets(DescriptorStage stage, const char **render_target_names, Sampler **samplers, u32 count)  {
        auto *input_render_target_descriptor = new VulkanInputRenderTargetDescriptor();
        for (u32 index = 0; index < count; index++) {
            input_render_target_descriptor->add_render_target(vulkan_context->get_render_pass().get_render_targets()[vulkan_context->get_render_pass().get_render_target_index_by_name(render_target_names[index])], (VulkanSampler *)samplers[index]);
        }
        descriptors.push_back(input_render_target_descriptor);
        setup_descriptor_binding(input_render_target_descriptor, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, stage, count);
        return _current_binding - 1;
    }

    void VulkanDescriptorSet::setup_descriptor_binding(VulkanDescriptor *descriptor, VkDescriptorType type, DescriptorStage stage, u32 count) {
        descriptor->set_binding_index(_current_binding);
        descriptor->set_count(count);
        descriptor->set_binding(VkDescriptorSetLayoutBinding {
            .binding = _current_binding,
            .descriptorType = type,
            .descriptorCount = count,
            .stageFlags = descriptor_stage2vk_shader_stage_flags(stage),
            .pImmutableSamplers = nullptr,
        });
        _current_binding++;
        g_descriptor_info[type]++;
    }

    VulkanDescriptorSet::VulkanDescriptorSet() {
        g_vulkan_descriptor_sets.push_back(this);
    }

    Result VulkanDescriptorSet::create() {
        component_create()

        STL_IMPL::vector<VkDescriptorSetLayoutBinding> bindings(descriptors.size());
        for (u32 index = 0; index < descriptors.size(); index++) {
            descriptors[index]->create();
            bindings[index] = descriptors[index]->get_binding();
        }
        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        descriptor_set_layout_create_info.pBindings = bindings.data();
        descriptor_set_layout_create_info.bindingCount = bindings.size();
        VK_CHECK(vkCreateDescriptorSetLayout(vulkan_context->get_device().get_logical_device(), &descriptor_set_layout_create_info, vulkan_context->get_allocator(), &layout))
        RHI_DEBUG("Create vulkan descriptor set layout -> 0x{:x}", (AddrType)layout)
        in_flight_descriptor_sets.resize(vulkan_context->get_max_in_flight_frame_count());

        return Result::eSuccess;
    }

    Result VulkanDescriptorSet::destroy() {
        component_destroy()

        in_flight_descriptor_sets.clear();

        RHI_DEBUG("Destroy vulkan descriptor set layout -> 0x{:x}", (AddrType)layout)
        vkDestroyDescriptorSetLayout(vulkan_context->get_device().get_logical_device(), layout, vulkan_context->get_allocator());

        for (auto &descriptor : descriptors) {
            descriptor->destroy();
        }

        return Result::eSuccess;
    }

    void *VulkanDescriptorSet::get_uniform_buffer_pointer(u32 binding, u32 index) {
        auto *descriptor = (VulkanUniformDescriptor *)descriptors[binding];
        return (void*)(((u8 *)descriptor->get_current_mapped_pointer()) + index * descriptor->get_size());
    }

    void VulkanDescriptorSet::set_texture(u32 binding, u32 index, Texture *texture) {
        auto *descriptor = (VulkanTextureDescriptor *)descriptors[binding];
        descriptor->set_texture(index, (VulkanTexture *)texture);
    }

    void VulkanDescriptorSet::set_input_render_target(u32 binding, u32 index, const char *render_target_name, Sampler *sampler) {
        auto *descriptor = (VulkanInputRenderTargetDescriptor *)descriptors[binding];
        descriptor->set_render_target(index, vulkan_context->get_render_pass().get_render_targets()[vulkan_context->get_render_pass().get_render_target_index_by_name(render_target_name)], (VulkanSampler *)sampler);
    }

    void VulkanDescriptorSet::update() {
        VK_CHECK(vkDeviceWaitIdle(vulkan_context->get_device().get_logical_device()))
        for (auto &descriptor : descriptors) {
            descriptor->update(this);
        }
    }
}
