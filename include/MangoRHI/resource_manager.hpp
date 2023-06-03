#pragma once

#include "commons.hpp"
#include "resource/render_target.hpp"
#include "resource/shader.hpp"
#include "resource/buffer.hpp"
#include "resource/texture.hpp"

namespace MangoRHI {
    class ResourceManager : public RuntimeComponent {
    public:
        virtual RenderTarget &create_render_target(const char *name, RenderTargetUsage usage) = 0;
        virtual Shader &create_shader(const char *filename) = 0;
        virtual VertexBuffer &create_vertex_buffer(u32 vertex_size, u32 count = 1024 * 1024) = 0;
        virtual IndexBuffer &create_index_buffer(u32 count = 1024 * 1024) = 0;
        virtual Texture &create_texture(const char *filename, u32 mipmap_levels = 0) = 0;
    };
}
