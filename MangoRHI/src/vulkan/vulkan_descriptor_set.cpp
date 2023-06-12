#include "vulkan_descriptor_set.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
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
            in_flight_buffers[in_flight_index] = std::make_unique<VulkanBuffer>();
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
        }
        in_flight_buffers.clear();
        mapped_pointers.clear();

        component_destroy_end()
    }

    void VulkanUniformDescriptor::update(const VulkanDescriptorSet &descriptor_set) {
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
            write_descriptor_set.dstSet = descriptor_set.get_in_flight_descriptor_sets()[in_flight_index];
            vkUpdateDescriptorSets(vulkan_context->get_device()->get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
        }
    }

    void *VulkanUniformDescriptor::get_current_mapped_pointer() const {
        return mapped_pointers[vulkan_context->get_current_in_flight_frame_index()];
    }

    void VulkanTextureDescriptor::add_texture(const VulkanTexture &texture) {
        textures.push_back(std::ref(texture));
    }

    void VulkanTextureDescriptor::set_texture(u32 index, const VulkanTexture &texture) {
        textures[index] = std::ref(texture);
    }

    Result VulkanTextureDescriptor::create() {
        component_create()

        return Result::eSuccess;
    }

    Result VulkanTextureDescriptor::destroy() {
        component_destroy()

        component_destroy_end()
    }

    void VulkanTextureDescriptor::update(const VulkanDescriptorSet &descriptor_set) {
        STL_IMPL::vector<VkDescriptorImageInfo> descriptor_image_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            descriptor_image_infos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptor_image_infos[index].imageView = textures[index].get().get_image().get_image_view();
            descriptor_image_infos[index].sampler = textures[index].get().get_sampler().get_sampler();
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pImageInfo = descriptor_image_infos.data();
        write_descriptor_set.descriptorType = get_binding().descriptorType;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstBinding = get_binding_index();
        write_descriptor_set.dstArrayElement = 0;
        for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
            write_descriptor_set.dstSet = descriptor_set.get_in_flight_descriptor_sets()[in_flight_index];
            vkUpdateDescriptorSets(vulkan_context->get_device()->get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
        }
    }

    Result VulkanInputRenderTargetDescriptor::create() {
        component_create()

        return Result::eSuccess;
    }

    Result VulkanInputRenderTargetDescriptor::destroy() {
        component_destroy()

        component_destroy_end()
    }

    void VulkanInputRenderTargetDescriptor::add_render_target(const VulkanRenderTarget &render_target, const VulkanSampler &sampler) {
        render_targets.push_back(render_target);
        samplers.push_back(sampler);
    }

    void VulkanInputRenderTargetDescriptor::set_render_target(u32 index, const VulkanRenderTarget &render_target, const VulkanSampler &sampler) {
        render_targets[index] = std::ref(render_target);
        samplers[index] = std::ref(sampler);
    }

    void VulkanInputRenderTargetDescriptor::update(const VulkanDescriptorSet &descriptor_set) {
        STL_IMPL::vector<VkDescriptorImageInfo> descriptor_image_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            descriptor_image_infos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            if (render_targets[index].get().get_is_each_frame_render_target() == MG_TRUE) {
                descriptor_image_infos[index].imageView = render_targets[index].get().get_image_views()[vulkan_context->get_swapchain()->get_image_index()];
            } else {
                descriptor_image_infos[index].imageView = render_targets[index].get().get_image_views()[0];
            }
            descriptor_image_infos[index].sampler = samplers[index].get().get_sampler();
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pImageInfo = descriptor_image_infos.data();
        write_descriptor_set.descriptorType = get_binding().descriptorType;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstBinding = get_binding_index();
        write_descriptor_set.dstArrayElement = 0;
        for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
            write_descriptor_set.dstSet = descriptor_set.get_in_flight_descriptor_sets()[in_flight_index];
            vkUpdateDescriptorSets(vulkan_context->get_device()->get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
        }
    }

    u32 VulkanDescriptorSet::add_uniforms_descriptor(DescriptorStage stage, u32 size, u32 count) {
        auto &uniform_descriptor = dynamic_cast<VulkanUniformDescriptor &>(*descriptors.emplace_back(new VulkanUniformDescriptor()));
        uniform_descriptor.set_size(size);
        setup_descriptor_binding(uniform_descriptor, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, stage, count);
        return _current_binding - 1;
    }

    u32 VulkanDescriptorSet::add_textures_descriptor(DescriptorStage stage, const STL_IMPL::vector<std::reference_wrapper<const Texture>> &textures) {
        auto &texture_descriptor = dynamic_cast<VulkanTextureDescriptor &>(*descriptors.emplace_back(new VulkanTextureDescriptor()));
        for (auto &texture : textures) {
            texture_descriptor.add_texture((VulkanTexture &)texture.get());
        }
        setup_descriptor_binding(texture_descriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage, textures.size());
        return _current_binding - 1;
    }

    u32 VulkanDescriptorSet::add_input_render_targets_descriptor(DescriptorStage stage, const STL_IMPL::vector<STL_IMPL::pair<const char *, const Sampler &>> &render_targets)  {
        auto &input_render_target_descriptor = dynamic_cast<VulkanInputRenderTargetDescriptor &>(*descriptors.emplace_back(new VulkanInputRenderTargetDescriptor()));
        for (auto &render_target : render_targets) {
            input_render_target_descriptor.add_render_target(*vulkan_context->get_render_pass()->get_render_targets()[vulkan_context->get_render_pass()->get_render_target_index_by_name(render_target.first)], (VulkanSampler &)render_target.second);
        }
        setup_descriptor_binding(input_render_target_descriptor, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, stage, render_targets.size());
        return _current_binding - 1;
    }

    void VulkanDescriptorSet::setup_descriptor_binding(VulkanDescriptor &descriptor, VkDescriptorType type, DescriptorStage stage, u32 count) {
        descriptor.set_binding_index(_current_binding);
        descriptor.set_count(count);
        descriptor.set_binding(VkDescriptorSetLayoutBinding {
            .binding = _current_binding,
            .descriptorType = type,
            .descriptorCount = count,
            .stageFlags = descriptor_stage2vk_shader_stage_flags(stage),
            .pImmutableSamplers = nullptr,
        });
        _current_binding++;
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
        VK_CHECK(vkCreateDescriptorSetLayout(vulkan_context->get_device()->get_logical_device(), &descriptor_set_layout_create_info, vulkan_context->get_allocator(), &layout))
        RHI_DEBUG("Create vulkan descriptor set layout -> 0x{:x}", (AddrType)layout)
        in_flight_descriptor_sets.resize(vulkan_context->get_max_in_flight_frame_count());

        vulkan_context->add_resource_recreate_callback([this]() {
            for (auto &descriptor : descriptors) {
                if (descriptor->get_type() == DescriptorType::eInputRenderTarget) {
                    descriptor->update(*this);
                }
            }
        });

        return Result::eSuccess;
    }

    Result VulkanDescriptorSet::destroy() {
        component_destroy()

        in_flight_descriptor_sets.clear();

        RHI_DEBUG("Destroy vulkan descriptor set layout -> 0x{:x}", (AddrType)layout)
        vkDestroyDescriptorSetLayout(vulkan_context->get_device()->get_logical_device(), layout, vulkan_context->get_allocator());

        for (auto &descriptor : descriptors) {
            descriptor->destroy();
        }

        component_destroy_end()
    }

    void *VulkanDescriptorSet::get_uniform_buffer_pointer(u32 binding, u32 index) {
        auto &descriptor = (VulkanUniformDescriptor &)*descriptors[binding];
        return (void*)(((u8 *)descriptor.get_current_mapped_pointer()) + index * descriptor.get_size());
    }

    void VulkanDescriptorSet::set_texture(u32 binding, u32 index, Texture &texture) {
        auto &descriptor = (VulkanTextureDescriptor &)*descriptors[binding];
        descriptor.set_texture(index, (VulkanTexture &)texture);
    }

    void VulkanDescriptorSet::set_input_render_target(u32 binding, u32 index, const STL_IMPL::pair<const char *, const Sampler &> &render_target) {
        auto &descriptor = (VulkanInputRenderTargetDescriptor &)*descriptors[binding];
        descriptor.set_render_target(index, *vulkan_context->get_render_pass()->get_render_targets()[vulkan_context->get_render_pass()->get_render_target_index_by_name(render_target.first)], (VulkanSampler &)render_target.second);
    }

    void VulkanDescriptorSet::update(u32 binding) {
        VK_CHECK(vkDeviceWaitIdle(vulkan_context->get_device()->get_logical_device()))
        descriptors[binding]->update(*this);
    }

    void VulkanDescriptorSet::update_all() {
        VK_CHECK(vkDeviceWaitIdle(vulkan_context->get_device()->get_logical_device()))
        for (auto &descriptor : descriptors) {
            descriptor->update(*this);
        }
    }
}
