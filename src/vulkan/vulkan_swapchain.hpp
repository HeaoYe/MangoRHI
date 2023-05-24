#pragma once

#include "vulkan_commons.hpp"
#include "swapchain.hpp"

namespace MangoRHI {
    class VulkanSwapchain final : public Swapchain {
    public:
        void set_image_count(u32 count);
        Result create() override;
        Result destroy() override;
        Result recreate() override;
        Result acquire_next_frame() override;
        Result present() override;

    define_member(u32, image_count, 3)
    define_member(u32, image_index, MANGO_NO_INIT_VAULE)
    define_member(VkSurfaceFormatKHR, format, MANGO_NO_INIT_VAULE)
    define_member(VkPresentModeKHR, present_mode, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VkImage>, images, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VkImageView>, image_views, MANGO_NO_INIT_VAULE)
    private:
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    };
}
