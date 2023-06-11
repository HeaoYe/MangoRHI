#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/resource_factory.hpp"
#include "resource/vulkan_render_target.hpp"
#include "resource/vulkan_shader.hpp"
#include "resource/vulkan_buffer.hpp"
#include "resource/vulkan_texture.hpp"
#include "resource/vulkan_shader_program.hpp"

namespace MangoRHI {
    class VulkanResourceFactory final : public ResourceFactory {
    public:
        std::unique_ptr<Shader> create_shader(const char *filename) override;
        std::unique_ptr<VertexBuffer> create_vertex_buffer(u32 vertex_size, u32 count) override;
        std::unique_ptr<IndexBuffer> create_index_buffer(u32 count) override;
        std::unique_ptr<Texture> create_texture(const char *filename, u32 mipmap_levels) override;
        std::unique_ptr<Texture> create_empty_texture(u32 mipmap_levels) override;
        std::unique_ptr<Sampler> create_sampler() override;
        std::unique_ptr<ShaderProgram> create_shader_program(const char *subpass_name) override;

    declare_component_cls(VulkanResourceFactory)
    };
}
