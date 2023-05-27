#include "vulkan_command.hpp"
#include "vulkan_context.hpp"

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

        VkSubmitInfo submit_info { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit_info.pWaitSemaphores = &vulkan_context->get_synchronization().get_image_available_semaphores()[vulkan_context->get_current_in_flight_frame_index()];
        submit_info.waitSemaphoreCount = 1;
        VkPipelineStageFlags stages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        };
        submit_info.pWaitDstStageMask = stages;
        submit_info.pCommandBuffers = &command_buffer;
        submit_info.commandBufferCount = 1;
        submit_info.pSignalSemaphores = &vulkan_context->get_synchronization().get_render_finished_semaphores()[vulkan_context->get_current_in_flight_frame_index()];
        submit_info.signalSemaphoreCount = 1;

        VK_CHECK(vkQueueSubmit(vulkan_context->get_device().get_graphics_queue(), 1, &submit_info, vulkan_context->get_synchronization().get_fences()[vulkan_context->get_current_in_flight_frame_index()]))

        return Result::eSuccess;
    }
}
