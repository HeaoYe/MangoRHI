#pragma once

#include "vulkan_commons.hpp"

namespace MangoRHI {
    class VulkanSynchronization final : public RuntimeComponent {
    public:
        const VkSemaphore &get_current_image_available_semaphore() const;
        const VkSemaphore &get_current_render_finished_semaphore() const;
        const VkFence &get_current_fence() const;

    define_private_member(STL_IMPL::vector<VkSemaphore>, image_available_semaphores, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkSemaphore>, render_finished_semaphores, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkFence>, fences, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanSynchronization)
    };
}
