#pragma once

#include "commons.hpp"
#include "resource/render_target.hpp"
#include "resource/shader.hpp"
#include "resource/buffer.hpp"
#include "resource/texture.hpp"
#include "resource/sampler.hpp"
#include "resource/shader_program.hpp"

namespace MangoRHI {
    template <typename T>
    struct ResourceDeleter {
        void operator()(T *resource) const {
            resource->destroy();
        }
    };

    class ResourceFactory : public RuntimeComponent {
    public:
        virtual std::unique_ptr<Shader, ResourceDeleter<Shader>> create_shader(const char *filename) = 0;
        virtual std::unique_ptr<VertexBuffer, ResourceDeleter<VertexBuffer>> create_vertex_buffer(u32 vertex_size, u32 count = 1024 * 1024) = 0;
        virtual std::unique_ptr<IndexBuffer, ResourceDeleter<IndexBuffer>> create_index_buffer(u32 count = 1024 * 1024) = 0;
        virtual std::unique_ptr<Texture, ResourceDeleter<Texture>> create_texture(const char *filename, u32 mipmap_levels = 0) = 0;
        virtual std::unique_ptr<Texture, ResourceDeleter<Texture>> create_empty_texture(u32 mipmap_levels = 0) = 0;
        virtual std::unique_ptr<Sampler, ResourceDeleter<Sampler>> create_sampler() = 0;
        virtual std::unique_ptr<ShaderProgram, ResourceDeleter<ShaderProgram>> create_shader_program(const char *subpass_name) = 0;
    };
}
