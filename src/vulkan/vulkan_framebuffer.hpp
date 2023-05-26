#pragma once

#include "vulkan_commons.hpp"

namespace MangoRHI {
    class VulkanFrameBuffer final : public RuntimeComponent {
    public:
        Result create() override;
        Result recreate();
        Result destroy() override;
    
    define_member(STL_IMPL::vector<VkFramebuffer>, framebuffers, MANGO_NO_INIT_VAULE)
    };
}
