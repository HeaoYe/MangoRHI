#pragma once

#include "../vulkan_commons.hpp"
#include "vulkan_buffer.hpp"

namespace MangoRHI {
    class VulkanImage final : public RuntimeComponent {
    public:
        Result create() override;
        Result destroy() override;
    
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC, VkExtent2D, extent, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, VkFormat, format, MANGO_NO_INIT_VAULE)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC, VkImageUsageFlags, usage, MANGO_NO_INIT_VAULE)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC, VkImageAspectFlags, aspect, MANGO_NO_INIT_VAULE)

    define_private_pointer(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkImage, image, VK_NULL_HANDLE)
    define_private_member(VkDeviceMemory, memory, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkImageView, image_view, VK_NULL_HANDLE)

    declare_component_cls(VulkanImage)
    };
}
