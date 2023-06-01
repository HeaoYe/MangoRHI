#include "vulkan_shader.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanShader::create() {
        component_create()

        auto code = read_binary_file(filename);
        VkShaderModuleCreateInfo shader_module_create_info { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        shader_module_create_info.pCode = reinterpret_cast<const u32 *>(code.data());
        shader_module_create_info.codeSize = code.size();
        VK_CHECK(vkCreateShaderModule(vulkan_context->get_device().get_logical_device(), &shader_module_create_info, vulkan_context->get_allocator(), &shader_module))
        RHI_DEBUG("Create vulkan shader module -> 0x{:x}", (AddrType)shader_module)

        return Result::eSuccess;
    }

    Result VulkanShader::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy vulkan shader module -> 0x{:x}", (AddrType)shader_module)
        vkDestroyShaderModule(vulkan_context->get_device().get_logical_device(), shader_module, vulkan_context->get_allocator());

        return Result::eSuccess;
    }
}
