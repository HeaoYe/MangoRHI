#pragma once

#include "vulkan_commons.hpp"

namespace MangoRHI {
    class VulkanDevice final : public RuntimeComponent {
    public:
        void set_name(const char *name);
        Result create() override;
        Result destroy() override;

        define_readonly_const_pointer_value(char, name, "")
        define_readonly_member(VkPhysicalDevice, physical_device)
        define_readonly_member(VkDevice, logical_device)
        define_readonly_member(u32, graphics_family_index)
        define_readonly_member(u32, present_family_index)
        define_readonly_member(u32, transfer_family_index)
        define_readonly_member(u32, compute_family_index)
        define_readonly_member(VkQueue, graphics_queue)
        define_readonly_member(VkQueue, present_queue)
        define_readonly_member(VkQueue, transfer_queue)
        define_readonly_member(VkQueue, compute_queue)
    private:
        Bool check_physical_device_suitable(const VkPhysicalDevice &physical_device);
    };
}
