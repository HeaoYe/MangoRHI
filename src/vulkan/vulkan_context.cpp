#include "vulkan_context.hpp"

namespace MangoRHI {
    VulkanContext *vulkan_context;

    void VulkanContext::set_api_info(const void *info) {
        this->info = (VulkanContextInfo *)info;
    }

    void VulkanContext::set_application_name(const char *name) {
        app_name = name;
    }

    void VulkanContext::set_device_name(const char *name) {
        device.set_name(name);
    }

    void VulkanContext::resize(const u32 width, const u32 height) {
        extent.width = width;
        extent.height = height;
    }

    Result VulkanContext::create() {
        component_create()
        vulkan_context = this;

        VkApplicationInfo app_info { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
        app_info.pApplicationName = app_name;
        app_info.apiVersion = VK_API_VERSION_1_3;
        VkInstanceCreateInfo instance_create_info { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instance_create_info.pApplicationInfo = &app_info;
        instance_create_info.ppEnabledExtensionNames = info->extensions;
        instance_create_info.enabledExtensionCount = info->extension_count;
        instance_create_info.ppEnabledLayerNames = nullptr;
        instance_create_info.enabledLayerCount = 0;
        VK_CHECK(vkCreateInstance(&instance_create_info, allocator, &instance))
        RHI_DEBUG("Create vulkan instance -> 0x{:x}", (AddrType)instance)

        surface = info->surface_create_callback(instance, allocator);
        if (surface == VK_NULL_HANDLE) {
            RHI_ERROR("Cannot create vulkan surface")
            return Result::eFailed;
        }
        RHI_DEBUG("Create vulkan surface -> 0x{:x}", (AddrType)surface)

        device.create();

        return Result::eSuccess;
    }

    Result VulkanContext::destroy() {
        component_destroy()

        device.destroy();

        RHI_DEBUG("Destroy Vulkan Instance -> 0x{:x}", (AddrType)instance)
        vkDestroyInstance(instance, allocator);

        return Result::eSuccess;
    }
}
