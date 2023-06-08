#include "vulkan_texture.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanTexture::create() {
        component_create()

        int width, height, channels;
        stbi_uc* pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
        extent.width = static_cast<u32>(width);
        extent.height = static_cast<u32>(height);
        if (mipmap_levels == 0) {
            mipmap_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
        }
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
        image.set_mipmap_levels(mipmap_levels);
        image.set_format(VK_FORMAT_R8G8B8A8_SRGB);
        image.set_usage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        image.set_aspect(VK_IMAGE_ASPECT_COLOR_BIT);
        image.create();

        VulkanCommand command;
        vulkan_context->transition_image_layout(image.get_image(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipmap_levels);

        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(vulkan_context->get_device().get_physical_device(), image.get_format(), &format_properties);
        if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            RHI_ERROR("Vulkan texture image format does not support linear blitting");
        }

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
            extent.width,
            extent.height,
            1
        };
        vkCmdCopyBufferToImage(command.get_command_buffer(), staging.get_buffer(), image.get_image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        vulkan_context->get_command_pool().free(&command);
        staging.destroy();

        vulkan_context->get_command_pool().allocate_single_use(&command);
        VkImageMemoryBarrier barrier {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image.get_image();
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;
        i32 mip_width = width, mip_height = height;
        for (u32 level = 1; level < mipmap_levels; level++) {
            barrier.subresourceRange.baseMipLevel = level - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            vkCmdPipelineBarrier(command.get_command_buffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            VkImageBlit blit {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mip_width, mip_height, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = level - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = level;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;
            vkCmdBlitImage(command.get_command_buffer(), image.get_image(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image.get_image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(command.get_command_buffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
            if (mip_width > 1) {
                mip_width /= 2;
            }
            if (mip_height > 1) {
                mip_height /= 2;
            }
        }
        barrier.subresourceRange.baseMipLevel = mipmap_levels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(command.get_command_buffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        vulkan_context->get_command_pool().free(&command);

        sampler.set_mipmap_levels(mipmap_levels);
        sampler.create();

        return Result::eSuccess;
    }

    Result VulkanTexture::destroy() {
        component_destroy()

        sampler.destroy();
        image.destroy();

        return Result::eSuccess;
    }

    Sampler &VulkanTexture::get_sampler_reference() {
        return sampler;
    }
}
