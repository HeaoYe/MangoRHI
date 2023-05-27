#pragma once

#include "vulkan_commons.hpp"
#include "vulkan_command.hpp"

namespace MangoRHI {
    class VulkanCommandPool final : public RuntimeComponent {
    public:
        Result create() override;
        Result destroy() override;
        
        void allocate(CommandLevel level, VulkanCommand *command) const;
        void allocate_single_use(VulkanCommand *command) const;
        void free(VulkanCommand *command) const;
    
    define_member(VkCommandPool, command_pool, VK_NULL_HANDLE)
    };
}
