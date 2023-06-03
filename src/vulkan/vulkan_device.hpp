#pragma once

#include "vulkan_commons.hpp"

namespace MangoRHI {
    class VulkanDevice final : public RuntimeComponent {
    public:
        Result create() override;
        Result destroy() override;

        void query_swapchain_details(SwapchainDetails &details) const;
        VkFormat get_supported_depth_format() const;
        VkSampleCountFlagBits get_max_multisample_count() const;

    private:
        Bool check_physical_device_suitable(const VkPhysicalDevice &physical_device);
    
    define_readonly_pointer(MANGO_CONST_GETTER, MANGO_SETTER, char, name, "")
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkPhysicalDevice, physical_device, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkDevice, logical_device, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, u32, graphics_family_index, -1u)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, u32, present_family_index, -1u)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, u32, transfer_family_index, -1u)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, u32, compute_family_index, -1u)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkQueue, graphics_queue, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkQueue, present_queue, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkQueue, transfer_queue, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkQueue, compute_queue, VK_NULL_HANDLE)
    
    declare_component_cls(VulkanDevice)
    };
}
