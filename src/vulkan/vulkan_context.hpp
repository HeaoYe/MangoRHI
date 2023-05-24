#pragma once

#include "context.hpp"
#include "vulkan_commons.hpp"
#include "vulkan_device.hpp"
#include "vulkan_swapchain.hpp"

namespace MangoRHI {
    class VulkanContext final : public Context {
    public:
        void set_api_info(const void *info) override;
        void set_application_name(const char *name) override;
        void set_device_name(const char *name) override;
        void set_swapchain_image_count(u32 count) override;
        void resize(const u32 width, const u32 height) override;
        Result create() override;
        Result destroy() override;

        VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect) const;
    private:
        Swapchain *get_swapchain() override;

    define_readonly_pointer(char, app_name, "")
    define_pointer(VkAllocationCallbacks, allocator, VK_NULL_HANDLE)
    define_member(VkInstance, instance, VK_NULL_HANDLE)
    define_member(VkSurfaceKHR, surface, VK_NULL_HANDLE)
    define_member(VulkanDevice, device, MANGO_NO_INIT_VAULE)
    define_member(VulkanSwapchain, swapchain, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VkExtent2D, extent, MANGO_NO_INIT_VAULE)

    private:
        const VulkanContextInfo *info;
    };

    extern VulkanContext *vulkan_context;
}
