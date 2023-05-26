#pragma once

#include "vulkan_commons.hpp"

namespace MangoRHI {
    class VulkanDevice final : public RuntimeComponent {
    public:
        void set_name(const char *name);
        Result create() override;
        Result destroy() override;

        void query_swapchain_details(SwapchainDetails &details) const;

    define_readonly_pointer(char, name, "")
    define_member(VkPhysicalDevice, physical_device, VK_NULL_HANDLE)
    define_member(VkDevice, logical_device, VK_NULL_HANDLE)
    define_member(u32, graphics_family_index, -1u)
    define_member(u32, present_family_index, -1u)
    define_member(u32, transfer_family_index, -1u)
    define_member(u32, compute_family_index, -1u)
    define_member(VkQueue, graphics_queue, VK_NULL_HANDLE)
    define_member(VkQueue, present_queue, VK_NULL_HANDLE)
    define_member(VkQueue, transfer_queue, VK_NULL_HANDLE)
    define_member(VkQueue, compute_queue, VK_NULL_HANDLE)
    private:
        Bool check_physical_device_suitable(const VkPhysicalDevice &physical_device);
    };
}
