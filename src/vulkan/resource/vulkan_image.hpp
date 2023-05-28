#pragma once

#include "../vulkan_commons.hpp"
#include "vulkan_buffer.hpp"

namespace MangoRHI {
    class VulkanImage final : public RuntimeComponent {
    public:
        void set_extent(u32 width, u32 height);

        Result create() override;
        Result destroy() override;
    
    define_private_member(u32, width, MANGO_NO_INIT_VAULE)
    define_private_member(u32, height, MANGO_NO_INIT_VAULE)
    define_private_pointer(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VkFormat, format, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VkImageUsageFlags, usage, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VkImageAspectFlags, aspect, MANGO_NO_INIT_VAULE)
    define_member(VkImage, image, VK_NULL_HANDLE)
    define_member(VkDeviceMemory, memory, VK_NULL_HANDLE)
    define_member(VkImageView, image_view, VK_NULL_HANDLE)

    no_copy_and_move_construction(VulkanImage)
    };
}
