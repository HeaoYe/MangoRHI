#include "vulkan_synchronization.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanSynchronization::create() {
        component_create()

        VkFenceCreateInfo fence_create_info { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkSemaphoreCreateInfo semaphore_create_info { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

        u32 max_in_flight_frame_count = vulkan_context->get_max_in_flight_frame_count();
        image_available_semaphores.resize(max_in_flight_frame_count);
        render_finished_semaphores.resize(max_in_flight_frame_count);
        fences.resize(max_in_flight_frame_count);
        for (u32 index = 0; index < max_in_flight_frame_count; index++) {
            VK_CHECK(vkCreateSemaphore(vulkan_context->get_device().get_logical_device(), &semaphore_create_info, vulkan_context->get_allocator(), &image_available_semaphores[index]))
            VK_CHECK(vkCreateSemaphore(vulkan_context->get_device().get_logical_device(), &semaphore_create_info, vulkan_context->get_allocator(), &render_finished_semaphores[index]))
            VK_CHECK(vkCreateFence(vulkan_context->get_device().get_logical_device(), &fence_create_info, vulkan_context->get_allocator(), &fences[index]))
        }

        return Result::eSuccess;
    }

    Result VulkanSynchronization::destroy() {
        component_destroy()

        for (u32 index = 0; index < vulkan_context->get_max_in_flight_frame_count(); index++) {
            vkDestroySemaphore(vulkan_context->get_device().get_logical_device(), image_available_semaphores[index], vulkan_context->get_allocator());
            vkDestroySemaphore(vulkan_context->get_device().get_logical_device(), render_finished_semaphores[index], vulkan_context->get_allocator());
            vkDestroyFence(vulkan_context->get_device().get_logical_device(), fences[index], vulkan_context->get_allocator());
        }

        image_available_semaphores.clear();
        render_finished_semaphores.clear();
        fences.clear();

        return Result::eSuccess;
    }
}
