#pragma once

#include "vulkan_commons.hpp"
#include "vulkan_descriptor_set.hpp"

namespace MangoRHI {
    class VulkanDescriptorPool final : public RuntimeComponent {
    public:
        Result create() override;
        Result destroy() override;
    
    define_member(VkDescriptorPool, descriptor_pool, VK_NULL_HANDLE)

    no_copy_and_move_construction(VulkanDescriptorPool)
    };
}
