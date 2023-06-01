#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/swapchain.hpp"
#include "resource/vulkan_render_target.hpp"

namespace MangoRHI {
    class VulkanSwapchain final : public Swapchain {
    public:
        Result create() override;
        Result destroy() override;
        Result acquire_next_frame() override;
        Result present() override;

    private:
        void recreate_all();

    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, image_count, 3)

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, u32, image_index, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkSurfaceFormatKHR, format, MANGO_NO_INIT_VAULE)
    define_private_member(VkPresentModeKHR, present_mode, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkImage>, images, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkImageView>, image_views, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, VulkanRenderTarget, render_target, MANGO_NO_INIT_VAULE)
    define_private_member(VkSwapchainKHR, swapchain, VK_NULL_HANDLE)

    declare_component_cls(VulkanSwapchain)
    };
}
