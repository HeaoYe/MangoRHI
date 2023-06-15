#include "vulkan_texture_descriptor.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanTextureDescriptor::add_texture(const VulkanTexture &texture) {
        textures.push_back(texture);
    }

    void VulkanTextureDescriptor::set_texture(u32 index, const VulkanTexture &texture) {
        textures[index] = texture;
    }

    Result VulkanTextureDescriptor::create() {
        component_create()

        textures.resize(get_count());

        return Result::eSuccess;
    }

    Result VulkanTextureDescriptor::destroy() {
        component_destroy()

        textures.clear();

        component_destroy_end()
    }

    void VulkanTextureDescriptor::update(const VulkanDescriptorSet &descriptor_set) {
        STL_IMPL::vector<VkDescriptorImageInfo> descriptor_image_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            if (textures[index].is_null() == MG_TRUE) {
                RHI_ERROR("Updata descriptor set without set_texture({}, ...)", index)
            }
            descriptor_image_infos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptor_image_infos[index].imageView = textures[index]->get_image().get_image_view();
            descriptor_image_infos[index].sampler = textures[index]->get_sampler().get_sampler();
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pImageInfo = descriptor_image_infos.data();
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstBinding = get_binding_index();
        write_descriptor_set.dstArrayElement = 0;
        for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
            write_descriptor_set.dstSet = descriptor_set.get_in_flight_descriptor_sets()[in_flight_index];
            vkUpdateDescriptorSets(vulkan_context->get_device()->get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
        }
    }
}
