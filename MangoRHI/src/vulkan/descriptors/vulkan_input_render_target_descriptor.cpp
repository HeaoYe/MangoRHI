#include "vulkan_input_render_target_descriptor.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanInputRenderTargetDescriptor::create() {
        component_create()

        render_targets.resize(get_count());
        samplers.resize(get_count());

        return Result::eSuccess;
    }

    Result VulkanInputRenderTargetDescriptor::destroy() {
        component_destroy()

        render_targets.clear();
        samplers.clear();

        component_destroy_end()
    }

    void VulkanInputRenderTargetDescriptor::add_render_target(const VulkanRenderTarget &render_target, const VulkanSampler &sampler) {
        render_targets.push_back(render_target);
        samplers.push_back(sampler);
    }

    void VulkanInputRenderTargetDescriptor::set_render_target(u32 index, const VulkanRenderTarget &render_target, const VulkanSampler &sampler) {
        render_targets[index] = render_target;
        samplers[index] = sampler;
    }

    void VulkanInputRenderTargetDescriptor::update(const VulkanDescriptorSet &descriptor_set) {
        STL_IMPL::vector<VkDescriptorImageInfo> descriptor_image_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            if (render_targets[index].is_null() == MG_TRUE) {
                RHI_ERROR("Updata descriptor set without set_render_target(..., {}, ...)", index)
            }
            descriptor_image_infos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            if (render_targets[index]->get_is_each_frame_render_target() == MG_TRUE) {
                descriptor_image_infos[index].imageView = render_targets[index]->get_image_views()[vulkan_context->get_swapchain()->get_image_index()];
            } else {
                descriptor_image_infos[index].imageView = render_targets[index]->get_image_views()[0];
            }
            descriptor_image_infos[index].sampler = samplers[index]->get_sampler();
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pImageInfo = descriptor_image_infos.data();
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstBinding = get_binding_index();
        write_descriptor_set.dstArrayElement = 0;
        for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
            write_descriptor_set.dstSet = descriptor_set.get_in_flight_descriptor_sets()[in_flight_index];
            vkUpdateDescriptorSets(vulkan_context->get_device()->get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
        }
    }
}
