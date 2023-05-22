#include "vulkan_context.hpp"

namespace MangoRHI {
    void VulkanContext::set_application_name(const char *name) {
        app_name = name;
    }

    void VulkanContext::set_device_name(const char *name) {
        device_name = name;
    }

    void VulkanContext::resize(u32 width, u32 height) {
        extent.width = width;
        extent.height = height;
    }

    Result VulkanContext::create() {
        component_create()

        VkApplicationInfo app_info { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
        app_info.pApplicationName = app_name;
        app_info.apiVersion = VK_API_VERSION_1_3;
        VkInstanceCreateInfo instance_create_info { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instance_create_info.pApplicationInfo = &app_info;
        instance_create_info.ppEnabledExtensionNames = nullptr;
        instance_create_info.enabledExtensionCount = 0;
        instance_create_info.ppEnabledLayerNames = nullptr;
        instance_create_info.enabledLayerCount = 0;
        VK_CHECK(vkCreateInstance(&instance_create_info, allocator, &instance))
        RHI_DEBUG("Create Vulkan Instance -> 0x{:x}", (AddrType)instance)

        return Result::eSuccess;
    }

    Result VulkanContext::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy Vulkan Instance -> 0x{:x}", (AddrType)instance)
        vkDestroyInstance(instance, allocator);

        return Result::eSuccess;
    }
}
