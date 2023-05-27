#pragma once

#include "context.hpp"
#include "vulkan_commons.hpp"
#include "vulkan_device.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_renderpass.hpp"
#include "vulkan_framebuffer.hpp"
#include "vulkan_synchronization.hpp"
#include "vulkan_command_pool.hpp"

namespace MangoRHI {
    class VulkanContext final : public Context {
    public:
        void set_api_info(const void *info) override;
        void set_application_name(const char *name) override;
        void set_device_name(const char *name) override;
        void set_swapchain_image_count(const u32 count) override;
        void set_max_in_flight_frame_count(const u32 count) override;
        void set_clear_color(ColorClearValue clear_color) override;
        void resize(const u32 width, const u32 height) override;
        Result create() override;
        Result destroy() override;

        Result begin_frame() override;
        Result end_frame() override;

        VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect) const;
        RenderPass &get_render_pass_reference() override { return render_pass; }
        Command &get_current_command() override { return commands[current_in_flight_frame_index]; };

    define_readonly_pointer(char, app_name, "")
    define_pointer(VkAllocationCallbacks, allocator, VK_NULL_HANDLE)
    define_member(VkInstance, instance, VK_NULL_HANDLE)
    define_member(VkSurfaceKHR, surface, VK_NULL_HANDLE)
    define_member(VulkanDevice, device, MANGO_NO_INIT_VAULE)
    define_member(VulkanSwapchain, swapchain, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VulkanRenderPass, render_pass, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VulkanFrameBuffer, framebuffer, MANGO_NO_INIT_VAULE)
    define_member(VulkanSynchronization, synchronization, MANGO_NO_INIT_VAULE)
    define_member(VulkanCommandPool, command_pool, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VkExtent2D, extent, MANGO_NO_INIT_VAULE)
    define_member(u32, max_in_flight_frame_count, 2)
    define_private_readonly_pointer(VulkanContextInfo, info, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanCommand>, commands, MANGO_NO_INIT_VAULE)
    define_member(u32, current_in_flight_frame_index, 0)
    };

    extern VulkanContext *vulkan_context;
}
