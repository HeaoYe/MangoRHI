#pragma once

#include "vulkan_commons.hpp"

namespace MangoRHI {
    class VulkanSynchronization final : public RuntimeComponent {
    public:
        Result create() override;
        Result destroy() override;
    
    define_member(STL_IMPL::vector<VkSemaphore>, image_available_semaphores, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VkSemaphore>, render_finished_semaphores, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VkFence>, fences, MANGO_NO_INIT_VAULE)
    };
}
