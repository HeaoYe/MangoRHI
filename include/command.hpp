#pragma once

#include "commons.hpp"
#include "resource/buffer.hpp"

namespace MangoRHI {
    class Command : public RuntimeComponent {
    public:
        virtual Result begin_render() = 0;
        virtual Result end_render() = 0;

        virtual void next_subpass() = 0;
        virtual void bind_vertex_buffer(const VertexBuffer *vertex_buffer) = 0;
        virtual void bind_index_buffer(const IndexBuffer *index_buffer) = 0;
        virtual void draw_instances(u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) = 0;
        virtual void draw_indexed_instances(u32 index_count, u32 instance_count, u32 first_index, u32 first_instance, u32 vertex_offset) = 0;
        virtual void set_viewport(Viewport &viewport) = 0;
        virtual void set_scissor(Scissor &scissor) = 0;
    };
}
