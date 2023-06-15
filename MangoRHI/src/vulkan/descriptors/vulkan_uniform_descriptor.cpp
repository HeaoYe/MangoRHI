#include "vulkan_uniform_descriptor.hpp"
#include "../vulkan_context.hpp"

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
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
}
