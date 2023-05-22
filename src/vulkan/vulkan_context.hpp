#pragma once

#include "vulkan_commons.hpp"
#include "context.hpp"

namespace MangoRHI {
    class VulkanContext final : public Context {
    public:
        void set_application_name(const char *name) override;
        void set_device_name(const char *name) override;
        void resize(u32 width, u32 height) override;
        Result create() override;
        Result destroy() override;
    private:
        const char *app_name;
        const char *device_name;
        VkExtent2D extent;

        VkAllocationCallbacks *allocator = nullptr;
        VkInstance instance;
        VkPhysicalDevice physical_device;
        VkDevice device;
    };
}
