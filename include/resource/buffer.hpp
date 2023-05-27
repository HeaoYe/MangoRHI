#pragma once

#include "commons.hpp"

namespace MangoRHI {
    class Buffer : public RuntimeComponent {
    public:
        virtual void set_size(const u32 size) = 0;
    
        virtual void write_data(const void *data, const u32 size, const u32 offset) = 0;
        virtual void resize(const u32 size) = 0;
        virtual void copy_from(const Buffer *other, const u64 src_offset, const u64 dst_offset, const u64 size) = 0;
    };

    class VertexBuffer : public Buffer {
    public:
        virtual void set_vertex_size(const u32 size) = 0;
    };

    class IndexBuffer : public Buffer {};
}
