#pragma once

#include "vulkan_commons.hpp"
#include "vulkan_descriptor_set.hpp"

namespace MangoRHI {
    class VulkanDescriptorPool final : public RuntimeComponent {
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkDescriptorPool, descriptor_pool, VK_NULL_HANDLE)

    declare_component_cls(VulkanDescriptorPool)
    };
}
