#include "vulkan_descriptor_set.hpp"
#include "vulkan_context.hpp"
#include "descriptors/vulkan_uniform_descriptor.hpp"
#include "descriptors/vulkan_texture_descriptor.hpp"
#include "descriptors/vulkan_input_render_target_descriptor.hpp"

namespace MangoRHI {
    u32 VulkanDescriptorSetLayout::add_uniforms_descriptor(DescriptorStageFlags stages, u32 size, u32 count) {
        bindings.push_back({
            .stages = stages,
            .type = DescriptorType::eUniform,
            .binding_index = _current_binding,
            .count = count,
            .data = { size },
        });
        return _current_binding ++;
    }

    u32 VulkanDescriptorSetLayout::add_textures_descriptor(DescriptorStageFlags stages, u32 count) {
        bindings.push_back({
            .stages = stages,
            .type = DescriptorType::eTexture,
            .binding_index = _current_binding,
            .count = count,
        });
        return _current_binding ++;
    }

    u32 VulkanDescriptorSetLayout::add_input_render_targets_descriptor(DescriptorStageFlags stages, u32 count)  {
        bindings.push_back({
            .stages = stages,
            .type = DescriptorType::eInputRenderTarget,
            .binding_index = _current_binding,
            .count = count,
        });
        return _current_binding ++;
    }

    Result VulkanDescriptorSetLayout::create() {
        component_create()

        STL_IMPL::vector<VkDescriptorSetLayoutBinding> vk_bindings(bindings.size());
        for (u32 index = 0; index < bindings.size(); index++) {
            vk_bindings[index].binding = bindings[index].binding_index;
            vk_bindings[index].descriptorType = descriptor_type2vk_descriptor_type(bindings[index].type);
            vk_bindings[index].descriptorCount = bindings[index].count;
            vk_bindings[index].stageFlags = descriptor_stage2vk_shader_stage_flags(bindings[index].stages);
            vk_bindings[index].pImmutableSamplers = nullptr;
        }

        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        descriptor_set_layout_create_info.pBindings = vk_bindings.data();
        descriptor_set_layout_create_info.bindingCount = vk_bindings.size();
        VK_CHECK(vkCreateDescriptorSetLayout(vulkan_context->get_device()->get_logical_device(), &descriptor_set_layout_create_info, vulkan_context->get_allocator(), &layout))
        RHI_DEBUG("Create vulkan descriptor set layout -> 0x{:x}", (AddrType)layout)

        return Result::eSuccess;
    }

    Result VulkanDescriptorSetLayout::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy vulkan descriptor set layout -> 0x{:x}", (AddrType)layout)
        vkDestroyDescriptorSetLayout(vulkan_context->get_device()->get_logical_device(), layout, vulkan_context->get_allocator());

        component_destroy_end()

        return Result::eSuccess;
    }

    Result VulkanDescriptorSet::create() {
        component_create()

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


    const VkDescriptorSet &VulkanDescriptorSet::get_current_in_flight_descriptor_set() const {
        return in_flight_descriptor_sets[vulkan_context->get_current_in_flight_frame_index()];
    }
}
