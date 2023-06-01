#include "vulkan_texture.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanTexture::bind_sampler(Sampler *sampler) {
        this->sampler = (VulkanSampler *)sampler;
    }

    Result VulkanTexture::create() {
        component_create()

        int width, height, channels;
        stbi_uc* pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
        extent.width = static_cast<u32>(width);
        extent.height = static_cast<u32>(height);
        VkDeviceSize size = width * height * 4;

        if (!pixels) {
            RHI_ERROR("Failed load texture file {}", filename)
            return Result::eFailed;
        }
        RHI_DEBUG("Load texture file {}, [{}, {}, {}]", filename, width, height, channels)
        
        staging.set_size(size);
        staging.set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        staging.set_properties(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        staging.create();
        staging.write_data(pixels, size, 0);
        stbi_image_free(pixels);

        image.set_extent(extent);
        image.set_format(VK_FORMAT_R8G8B8A8_SRGB);
        image.set_usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        image.set_aspect(VK_IMAGE_ASPECT_COLOR_BIT);
        image.create();

        vulkan_context->transition_image_layout(image.get_image(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        VulkanCommand command;
        vulkan_context->get_command_pool().allocate_single_use(&command);
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource = VkImageSubresourceLayers {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            static_cast<u32>(width),
            static_cast<u32>(height),
            1
        };
        vkCmdCopyBufferToImage(command.get_command_buffer(), staging.get_buffer(), image.get_image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        vulkan_context->get_command_pool().free(&command);
        vulkan_context->transition_image_layout(image.get_image(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        return Result::eSuccess;
    }

    Result VulkanTexture::destroy() {
        component_destroy()

        image.destroy();
        staging.destroy();

        return Result::eSuccess;
    }
}
