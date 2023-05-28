#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/swapchain.hpp"
#include "resource/vulkan_render_target.hpp"

namespace MangoRHI {
    class VulkanSwapchain final : public Swapchain {
    public:
        void set_image_count(u32 count);
        Result create() override;
        Result destroy() override;
        Result recreate() override;
        Result acquire_next_frame() override;
        Result present() override;

    private:
        void recreate_all();

    define_member(u32, image_count, 3)
    define_member(u32, image_index, MANGO_NO_INIT_VAULE)
    define_member(VkSurfaceFormatKHR, format, MANGO_NO_INIT_VAULE)
    define_member(VkPresentModeKHR, present_mode, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkImage>, images, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkImageView>, image_views, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VulkanRenderTarget, render_target, MANGO_NO_INIT_VAULE)
    define_private_member(VkSwapchainKHR, swapchain, VK_NULL_HANDLE)

    no_copy_and_move_construction(VulkanSwapchain)
    };
}
