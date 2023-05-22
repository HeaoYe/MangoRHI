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

    Result initialize_vulkan();
    Result quit_vulkan();
}
