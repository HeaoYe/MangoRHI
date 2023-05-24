#pragma once

#include "commons.hpp"
#include <vulkan/vulkan.h>

namespace MangoRHI {
    #define VK_CHECK(expr) \
    { \
        if (expr != VK_SUCCESS) { \
            RHI_ERROR("Vulkan Error: \n{} --> {}\n\t{}", __FILE__, __LINE__, #expr) \
            RHI_ERROR("") \
        } \
    }

    using SurfaceCreateFunc = std::function<VkSurfaceKHR(VkInstance &, VkAllocationCallbacks *)>;
    struct VulkanContextInfo {
        const char **extensions;
        u32 extension_count;
        SurfaceCreateFunc surface_create_callback;
    };

    struct SwapchainDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        STL_IMPL::vector<VkSurfaceFormatKHR> formats;
        STL_IMPL::vector<VkPresentModeKHR> present_modes;
    };

    Result initialize_vulkan();
    Result quit_vulkan();
}
