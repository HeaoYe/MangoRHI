#pragma once

#include "../vulkan_commons.hpp"
#include "resource/shader.hpp"

namespace MangoRHI {
    class VulkanShader final : public Shader {
    public:
        void set_filename(const char *filename) override;
        Result create() override;
        Result destroy() override;
    
    define_private_readonly_pointer(char, filename, "")
    define_member(VkShaderModule, shader_module, VK_NULL_HANDLE)
    };
}
