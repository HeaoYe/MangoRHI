#include "vulkan_command.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
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
        else {
            VK_CHECK(vkResetCommandBuffer(command_buffer, 0))
        }
        VK_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info))

        return Result::eSuccess;
    }

    Result VulkanCommand::end_render() {
        VK_CHECK(vkEndCommandBuffer(command_buffer))

        VkSubmitInfo submit_info { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit_info.pCommandBuffers = &command_buffer;
        submit_info.commandBufferCount = 1;
        if (is_single_use == MG_FALSE) {
            submit_info.pWaitSemaphores = &vulkan_context->get_synchronization()->get_current_image_available_semaphore();
            submit_info.waitSemaphoreCount = 1;
            VkPipelineStageFlags stages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            };
            submit_info.pWaitDstStageMask = stages;
            submit_info.pSignalSemaphores = &vulkan_context->get_synchronization()->get_current_render_finished_semaphore();
            submit_info.signalSemaphoreCount = 1;
            VK_CHECK(vkQueueSubmit(vulkan_context->get_device()->get_graphics_queue(), 1, &submit_info, vulkan_context->get_synchronization()->get_current_fence()))
        }
        else {
            VK_CHECK(vkQueueSubmit(vulkan_context->get_device()->get_transfer_queue(), 1, &submit_info, VK_NULL_HANDLE))
            VK_CHECK(vkQueueWaitIdle(vulkan_context->get_device()->get_transfer_queue()))
        }

        return Result::eSuccess;
    }

    void VulkanCommand::next_subpass() {
        vkCmdNextSubpass(command_buffer, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommand::bind_shader_program(const ShaderProgram &shader_program) {
        VulkanShaderProgram &vulkan_shader_program = (VulkanShaderProgram &)shader_program;
        auto bind_point = vulkan_context->get_render_pass()->get_subpasses()[vulkan_shader_program.get_subpass_index()]->get_bind_point();
        vkCmdBindPipeline(command_buffer, bind_point, vulkan_shader_program.get_pipeline());
        if (vulkan_shader_program.get_current_in_flight_descriptor_sets().size() > 0) {
            vkCmdBindDescriptorSets(command_buffer, bind_point, vulkan_shader_program.get_layout(), 0, vulkan_shader_program.get_current_in_flight_descriptor_sets().size(), vulkan_shader_program.get_current_in_flight_descriptor_sets().data(), 0, 0);
        }
    }

    void VulkanCommand::bind_vertex_buffer(const VertexBuffer &vertex_buffer, u32 binding) {
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(command_buffer, binding, 1, &((VulkanVertexBuffer &)vertex_buffer).get_buffer().get_buffer(), &offset);
    }

    void VulkanCommand::bind_index_buffer(const IndexBuffer &index_buffer) {
        vkCmdBindIndexBuffer(command_buffer, ((VulkanIndexBuffer &)index_buffer).get_buffer().get_buffer(), 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommand::draw_instances(u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) {
        vkCmdDraw(command_buffer, vertex_count, instance_count, first_vertex, first_instance);
    }

    void VulkanCommand::draw_indexed_instances(u32 index_count, u32 instance_count, u32 first_index, u32 first_instance, u32 vertex_offset) {
        vkCmdDrawIndexed(command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    void VulkanCommand::set_viewport(const Viewport &viewport) {
        VkViewport vk_viewport {
            .x = viewport.x,
            .y = viewport.y,
            .width = viewport.width,
            .height = viewport.height,
            .minDepth = viewport.min_depth,
            .maxDepth = viewport.max_depth,
        };
        vkCmdSetViewport(command_buffer, 0, 1, &vk_viewport);
    }

    void VulkanCommand::set_scissor(const Scissor &scissor) {
        VkRect2D vk_scissor{
            .offset = { .x = scissor.x, .y = scissor.y },
            .extent = { .width = scissor.width, .height = scissor.height },
        };
        vkCmdSetScissor(command_buffer, 0, 1, &vk_scissor);
    }
}
