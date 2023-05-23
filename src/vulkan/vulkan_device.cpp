#include "vulkan_device.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    void VulkanDevice::set_name(const char *name) {
        this->name = name;
    }

    Result VulkanDevice::create() {
        u32 device_count;
        VK_CHECK(vkEnumeratePhysicalDevices(vulkan_context->get_instance(), &device_count, nullptr))
        VkPhysicalDevice physical_devices[device_count];
        VK_CHECK(vkEnumeratePhysicalDevices(vulkan_context->get_instance(), &device_count, physical_devices))
        Bool found_device = MG_FALSE;
        if (strlen(name) > 0) {
            Bool found_device_name = MG_FALSE;
            VkPhysicalDeviceProperties properties;
            for (const auto &a_physical_device : physical_devices) {
                vkGetPhysicalDeviceProperties(a_physical_device, &properties);
                if (strcmp(properties.deviceName, name) == 0) {
                    found_device_name = MG_TRUE;
                    RHI_DEBUG("Found {}, check if it is suitable", properties.deviceName)
                    found_device = check_physical_device_suitable(a_physical_device);
                    if (found_device == MG_FALSE) {
                        RHI_DEBUG("{} is not suitable, default to selecting one", properties.deviceName)
                    }
                    break;
                }
            }
            if (found_device_name == MG_FALSE) {
                RHI_DEBUG("Cannot found {} -- skipping", name)
            }
        } else {
            RHI_DEBUG("No device name specified, default to selecting one")
        }
        
        if (found_device == MG_FALSE) {
            for (const auto &a_physical_device : physical_devices) {
                if (check_physical_device_suitable(a_physical_device)) {
                    found_device = MG_TRUE;
                    break;
                }
            }
        }

        if (found_device == MG_FALSE) {
            RHI_ERROR("No device is suitable")
            return Result::eDeviceNotFound;
        }

        return Result::eSuccess;
    }

    Bool VulkanDevice::check_physical_device_suitable(const VkPhysicalDevice &physical_device) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_device, &properties);
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_device, &features);
        
        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            RHI_DEBUG("{} is not a discrete gpu -- skipping", properties.deviceName)
            return MG_FALSE;
        }

        if (!features.samplerAnisotropy) {
            RHI_DEBUG("{} does not support sampler anisotropy -- skipping", properties.deviceName)
            return MG_FALSE;
        }

        u32 queue_family_count;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
        VkQueueFamilyProperties queue_families_properties[queue_family_count];
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families_properties);
        queue_family_count = 0;
        Bool found_family = MG_FALSE;
        for (const auto &queue_family_properties : queue_families_properties) {
            if (queue_family_properties.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT)) {
                VkBool32 supported;
                VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_count, vulkan_context->get_surface(), &supported))
                if (supported) {
                    graphics_family_index = queue_family_count;
                    present_family_index = queue_family_count;
                    compute_family_index = queue_family_count;
                    transfer_family_index = queue_family_count;
                    found_family = MG_TRUE;
                    break;
                }
            }
            queue_family_count++;
        }

        if (found_family == MG_FALSE) {
            RHI_DEBUG("{} has no suitable queue family -- skipping", properties.deviceName)
            return MG_FALSE;
        }

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &memoryProperties);
        RHI_DEBUG("{} is suitable", properties.deviceName)
        RHI_DEBUG("Device Driver Version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion), VK_VERSION_PATCH(properties.driverVersion))
        RHI_DEBUG("Vulkan API Version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion))
        for (u32 i = 0; i < memoryProperties.memoryHeapCount; i++) {
            f32 size = ((f32)memoryProperties.memoryHeaps[i].size) / 1024.0f / 1024.0f / 1024.0f;
            if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                RHI_DEBUG("Local GPU Memory: {} Gib", size)
            }
            else {
                RHI_DEBUG("Shared GPU Memory: {} Gib", size)
            }
        }

        return MG_TRUE;
    }

    Result VulkanDevice::destroy() {
        return Result::eSuccess;
    }
}
