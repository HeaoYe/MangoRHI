#include "vulkan_device.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanDevice::create() {
        component_create()

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

        STL_IMPL::set<u32> queue_family_indices = { graphics_family_index, present_family_index, transfer_family_index, compute_family_index };
        VkDeviceQueueCreateInfo queue_create_infos[queue_family_indices.size()];
        u32 i = 0;
        f32 priorities = 1.0f;
        for (const auto &queue_family_index : queue_family_indices) {
            queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[i].pNext = nullptr;
            queue_create_infos[i].flags = 0;
            queue_create_infos[i].queueFamilyIndex = queue_family_index;
            queue_create_infos[i].queueCount = 1;
            queue_create_infos[i].pQueuePriorities = &priorities;
            i++;
        }

        VkPhysicalDeviceFeatures features {};
        features.samplerAnisotropy = VK_TRUE;
        features.depthClamp = VK_TRUE;

        VkDeviceCreateInfo device_create_info { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        device_create_info.pQueueCreateInfos = queue_create_infos;
        device_create_info.queueCreateInfoCount = queue_family_indices.size();
        const char *extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        device_create_info.ppEnabledExtensionNames = extensions;
        device_create_info.enabledExtensionCount = 1;
        #if defined (MANGORHI_DEBUG)
        const char *layers[] = { "VK_LAYER_KHRONOS_validation" };
        device_create_info.ppEnabledLayerNames = layers;
        device_create_info.enabledLayerCount = 1;
        #else
        device_create_info.enabledLayerCount = 0;
        #endif
        device_create_info.pEnabledFeatures = &features;
        VK_CHECK(vkCreateDevice(physical_device, &device_create_info, vulkan_context->get_allocator(), &logical_device))
        RHI_DEBUG("Create vulkan device -> 0x{:x}", (AddrType)logical_device)

        vkGetDeviceQueue(logical_device, graphics_family_index, 0, &graphics_queue);
        vkGetDeviceQueue(logical_device, present_family_index, 0, &present_queue);
        vkGetDeviceQueue(logical_device, transfer_family_index, 0, &transfer_queue);
        vkGetDeviceQueue(logical_device, compute_family_index, 0, &compute_queue);

        return Result::eSuccess;
    }

    void VulkanDevice::query_swapchain_details(SwapchainDetails &details) const {
        u32 count;
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, vulkan_context->get_surface(), &details.capabilities))
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, vulkan_context->get_surface(), &count, nullptr))
        details.formats.resize(count);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, vulkan_context->get_surface(), &count, details.formats.data()))
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, vulkan_context->get_surface(), &count, nullptr))
        details.present_modes.resize(count);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, vulkan_context->get_surface(), &count, details.present_modes.data()))
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

        RHI_DEBUG("{} is suitable", properties.deviceName)
        this->physical_device = physical_device;
        RHI_INFO("Select Device {}", properties.deviceName)

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &memoryProperties);
        RHI_DEBUG("\tDevice Driver Version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion), VK_VERSION_PATCH(properties.driverVersion))
        RHI_DEBUG("\tVulkan API Version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion))
        for (u32 i = 0; i < memoryProperties.memoryHeapCount; i++) {
            f32 size = ((f32)memoryProperties.memoryHeaps[i].size) / 1024.0f / 1024.0f / 1024.0f;
            if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                RHI_DEBUG("\tLocal GPU Memory: {} Gib", size)
            }
            else {
                RHI_DEBUG("\tShared GPU Memory: {} Gib", size)
            }
        }

        return MG_TRUE;
    }

    Result VulkanDevice::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy vulkan device -> 0x{:x}", (AddrType)logical_device)
        vkDestroyDevice(logical_device, vulkan_context->get_allocator());

        return Result::eSuccess;
    }

    VkFormat VulkanDevice::get_supported_depth_format() const {
        return vulkan_context->find_supported_format({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    VkSampleCountFlagBits VulkanDevice::get_max_multisample_count() const {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_device, &properties);
        VkSampleCountFlags counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_32_BIT) {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_16_BIT) {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_8_BIT) {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_4_BIT) {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_2_BIT) {
            return VK_SAMPLE_COUNT_2_BIT;
        }
        return VK_SAMPLE_COUNT_1_BIT;
    }
}
