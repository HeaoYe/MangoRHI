#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/shader.hpp"

namespace MangoRHI {
    class VulkanShader final : public Shader {
    public:
        Result create() override;
        Result destroy() override;
    
    define_readonly_pointer(MANGO_NO_GETTER, MANGO_SETTER_OVERRIDE, char, filename, "")
    
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkShaderModule, shader_module, VK_NULL_HANDLE)

    declare_component_cls(VulkanShader)
    };
}
