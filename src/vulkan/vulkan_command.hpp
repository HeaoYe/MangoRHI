#pragma once

#include "vulkan_commons.hpp"
#include "command.hpp"

namespace MangoRHI {
    class VulkanCommand final : public Command {
    public:
        void set_single_use(Bool is_single_use);
        void set_command_buffer(VkCommandBuffer command_buffer);

        Result create() override;
        Result destroy() override;
        Result begin_render() override;
        Result end_render() override;

        void next_subpass() override;
        // void set_viewport() override;
        // void set_scissor() override;
    
    define_member(VkCommandBuffer, command_buffer, VK_NULL_HANDLE)
    define_member(Bool, is_single_use, MG_FALSE)
    define_private_member(u32, _current_subpass, 0)
    };
}
