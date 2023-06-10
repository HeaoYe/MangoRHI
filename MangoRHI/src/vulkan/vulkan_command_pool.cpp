#include "vulkan_command_pool.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanCommandPool::create() {
        component_create()

        VkCommandPoolCreateInfo command_pool_create_info { .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        command_pool_create_info.queueFamilyIndex = vulkan_context->get_device()->get_graphics_family_index();
        VK_CHECK(vkCreateCommandPool(vulkan_context->get_device()->get_logical_device(), &command_pool_create_info, vulkan_context->get_allocator(), &command_pool))
        RHI_DEBUG("Create vulkan command pool -> 0x{:x}", (AddrType)command_pool)

        return Result::eSuccess;
    }

    Result VulkanCommandPool::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy vulkan command pool -> 0x{:x}", (AddrType)command_pool)
        vkDestroyCommandPool(vulkan_context->get_device()->get_logical_device(), command_pool, vulkan_context->get_allocator());

        return Result::eSuccess;
    }

    void VulkanCommandPool::allocate(CommandLevel level, VulkanCommand &command) const {
        VkCommandBuffer command_buffer;
        VkCommandBufferAllocateInfo command_buffer_allocate_info { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        command_buffer_allocate_info.commandPool = command_pool;
        command_buffer_allocate_info.level = command_level2vk_command_buffer_level(level);
        command_buffer_allocate_info.commandBufferCount = 1;
        VK_CHECK(vkAllocateCommandBuffers(vulkan_context->get_device()->get_logical_device(), &command_buffer_allocate_info, &command_buffer))
        RHI_TRACE("Allocate vulkan command buffer -> 0x{:x}", (AddrType)command_buffer)
        command.command_buffer = command_buffer;
        command.is_single_use = MG_FALSE;
        command.create();
    }

    void VulkanCommandPool::allocate_single_use(VulkanCommand &command) const {
        allocate(CommandLevel::ePrimary, command);
        command.is_single_use = MG_TRUE;
        command.begin_render();
    }

    void VulkanCommandPool::free(VulkanCommand &command) const {
        if (command.get_is_single_use() == MG_TRUE) {
            command.end_render();
        }
        command.destroy();
        RHI_TRACE("Free vulkan command buffer -> 0x{:x}", (AddrType)command.get_command_buffer())
        vkFreeCommandBuffers(vulkan_context->get_device()->get_logical_device(), command_pool, 1, &command.get_command_buffer());
    }
}
