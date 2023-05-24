#include "vulkan_context.hpp"

namespace MangoRHI {
    VulkanContext *vulkan_context;

    void VulkanContext::set_api_info(const void *info) {
        this->info = (const VulkanContextInfo *)info;
    }

    void VulkanContext::set_application_name(const char *name) {
        app_name = name;
    }

    void VulkanContext::set_device_name(const char *name) {
        device.set_name(name);
    }

    void VulkanContext::set_swapchain_image_count(u32 count) {
        swapchain.set_image_count(count);
    }

    void VulkanContext::resize(const u32 width, const u32 height) {
        swapchain.recreate();
        RHI_DEBUG("Resize to [{}, {}]", extent.width, extent.height)
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
        #if defined (MANGO_DEBUG)
        const char *layers[] = { "VK_LAYER_KHRONOS_validation" };
        instance_create_info.ppEnabledLayerNames = layers;
        instance_create_info.enabledLayerCount = 1;
        #else
        instance_create_info.enabledLayerCount = 0;
        #endif
        VK_CHECK(vkCreateInstance(&instance_create_info, allocator, &instance))
        RHI_DEBUG("Create vulkan instance -> 0x{:x}", (AddrType)instance)

        surface = info->surface_create_callback(instance, allocator);
        if (surface == VK_NULL_HANDLE) {
            RHI_ERROR("Cannot create vulkan surface")
            return Result::eFailed;
        }
        RHI_DEBUG("Create vulkan surface -> 0x{:x}", (AddrType)surface)

        device.create();
        swapchain.create();

        return Result::eSuccess;
    }

    Result VulkanContext::destroy() {
        component_destroy()


        swapchain.destroy();
        device.destroy();

        RHI_DEBUG("Destroy vulkan surface -> 0x{:x}", (AddrType)surface)
        vkDestroySurfaceKHR(instance, surface, allocator);

        RHI_DEBUG("Destroy vulkan instance -> 0x{:x}", (AddrType)instance)
        vkDestroyInstance(instance, allocator);

        return Result::eSuccess;
    }

    Swapchain *VulkanContext::get_swapchain() {
        return (Swapchain *)&swapchain;
    }

    VkImageView VulkanContext::create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect) const {
        VkImageView image_view;
        VkImageViewCreateInfo image_view_create_info { .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        image_view_create_info.image = image;
        image_view_create_info.format = format;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.components = VkComponentMapping {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A,
        };
        image_view_create_info.subresourceRange = VkImageSubresourceRange {
            .aspectMask = aspect,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };
        VK_CHECK(vkCreateImageView(device.get_logical_device(), &image_view_create_info, allocator, &image_view))
        return image_view;
    }
}
