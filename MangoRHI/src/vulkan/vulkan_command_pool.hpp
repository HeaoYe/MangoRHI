#pragma once

#include "vulkan_commons.hpp"
#include "vulkan_command.hpp"

namespace MangoRHI {
    class VulkanCommandPool final : public RuntimeComponent {
    public:
        void allocate(CommandLevel level, VulkanCommand &command) const;
        void allocate_single_use(VulkanCommand &command) const;
        void free(VulkanCommand &command) const;

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkCommandPool, command_pool, VK_NULL_HANDLE)

    declare_component_cls(VulkanCommandPool)
    };
}
