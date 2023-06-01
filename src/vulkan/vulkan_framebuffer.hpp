#pragma once

#include "vulkan_commons.hpp"

namespace MangoRHI {
    class VulkanFrameBuffer final : public RuntimeComponent {
    public:
        Result create() override;
        Result destroy() override;
    
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkFramebuffer>, framebuffers, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanFrameBuffer)
    };
}
