#include "vulkan_command.hpp"

namespace MangoRHI {
    void VulkanCommand::set_single_use(Bool is_single_use) {
        this->is_single_use = is_single_use;
    }

    void VulkanCommand::set_command_buffer(VkCommandBuffer command_buffer) {
        this->command_buffer = command_buffer;
    }

    Result VulkanCommand::create() {
        component_create()

        return Result::eSuccess;
    }

    Result VulkanCommand::destroy() {
        component_destroy()

        return Result::eSuccess;
    }

    Result VulkanCommand::begin_render() {
        VkCommandBufferBeginInfo begin_info { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        if (is_single_use == MG_TRUE) {
            begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }
        VK_CHECK(vkResetCommandBuffer(command_buffer, 0))
        VK_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info))
        
        return Result::eSuccess;
    }

    Result VulkanCommand::end_render() {
        VK_CHECK(vkEndCommandBuffer(command_buffer))

        return Result::eSuccess;
    }
}
