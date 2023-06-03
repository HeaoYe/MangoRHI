#include "vulkan_image.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanImage::create() {
        component_create()

        VkImageCreateInfo image_create_info { .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.arrayLayers = 1;
        image_create_info.extent = { extent.width, extent.height, 1 };
        image_create_info.mipLevels = mipmap_levels;
        image_create_info.format = format;
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.usage = usage;
        VK_CHECK(vkCreateImage(vulkan_context->get_device().get_logical_device(), &image_create_info, vulkan_context->get_allocator(), &image))
        RHI_DEBUG("Create vulkan image -> 0x{:x}", (AddrType)image)

        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(vulkan_context->get_device().get_logical_device(), image, &requirements);

        VkMemoryAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocate_info.allocationSize = requirements.size;
        allocate_info.memoryTypeIndex = vulkan_context->find_memory_index(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        VK_CHECK(vkAllocateMemory(vulkan_context->get_device().get_logical_device(), &allocate_info, vulkan_context->get_allocator(), &memory));
        RHI_DEBUG("Allocate vulkan device memory -> 0x{:x}", (AddrType)memory)

        vkBindImageMemory(vulkan_context->get_device().get_logical_device(), image, memory, 0);

        image_view = vulkan_context->create_image_view(image, format, aspect, mipmap_levels);
        RHI_DEBUG("Create vulkan image view -> 0x{:x}", (AddrType)image_view)

        return Result::eSuccess;
    }

    Result VulkanImage::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy vulkan image view -> 0x{:x}", (AddrType)image_view)
        vkDestroyImageView(vulkan_context->get_device().get_logical_device(), image_view, vulkan_context->get_allocator());

        RHI_DEBUG("Free vulkan device memory -> 0x{:x}", (AddrType)memory)
        vkFreeMemory(vulkan_context->get_device().get_logical_device(), memory, vulkan_context->get_allocator());

        RHI_DEBUG("Destroy vulkan image -> 0x{:x}", (AddrType)image)
        vkDestroyImage(vulkan_context->get_device().get_logical_device(), image, vulkan_context->get_allocator());

        return Result::eSuccess;
    }
}
