#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/resource_manager.hpp"
#include "resource/vulkan_render_target.hpp"
#include "resource/vulkan_shader.hpp"
#include "resource/vulkan_buffer.hpp"
#include "resource/vulkan_texture.hpp"

namespace MangoRHI {
    class VulkanResourceManager final : public ResourceManager {
    public:
        RenderTarget &create_render_target(const char *name, RenderTargetUsage usage) override;
        Shader &create_shader(const char *filename) override;
        VertexBuffer &create_vertex_buffer(u32 vertex_size, u32 count) override;
        IndexBuffer &create_index_buffer(u32 count) override;
        Texture &create_texture(const char *filename, u32 mipmap_levels) override;

        void recreate_render_targets();

    define_private_member(STL_IMPL::vector<VulkanRenderTarget *>, render_targets, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanShader *>, shaders, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanVertexBuffer *>, vertex_buffers, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanIndexBuffer *>, index_buffers, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanTexture *>, textures, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanResourceManager)
    };
}
