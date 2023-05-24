#pragma once

#include "context.hpp"
#include "vulkan_commons.hpp"
#include "vulkan_device.hpp"

namespace MangoRHI {
    class VulkanContext final : public Context {
    public:
        void set_api_info(const void *info) override;
        void set_application_name(const char *name) override;
        void set_device_name(const char *name) override;
        void resize(const u32 width, const u32 height) override;
        Result create() override;
        Result destroy() override;

    define_readonly_const_pointer_value(char, app_name, "")
    define_readonly_pointer(VkAllocationCallbacks, allocator)
    define_readonly_member(VkInstance, instance)
    define_readonly_member(VkSurfaceKHR, surface)
    define_readonly_member(VulkanDevice, device)
    define_readonly_member(VkExtent2D, extent)

    private:
        VulkanContextInfo *info;
    };

    extern VulkanContext *vulkan_context;
}
