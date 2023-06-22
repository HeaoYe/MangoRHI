#pragma once

#include "../commons.hpp"

namespace MangoRHI {
    class Buffer : public RuntimeComponent {
    public:
        virtual void set_size(u32 size) = 0;

        virtual void *map() = 0;
        virtual void unmap() = 0;
        virtual void write_data(const void *data, u32 offset, u32 size) = 0;
        virtual void resize(u32 size) = 0;
        virtual void copy_from(const Buffer &other, u64 src_offset, u64 dst_offset, u64 size) = 0;
    };

    class VertexBuffer : public RuntimeComponent {
    public:
        virtual void set_vertex_size(u32 size) = 0;
        virtual void set_count(u32 count) = 0;

        virtual void *map() = 0;
        virtual void unmap() = 0;
        virtual void flush(u32 offset_count, u32 count) = 0;
        virtual void write_data(const void *data, u32 offset_count, u32 count) = 0;
        virtual void resize(u32 count) = 0;
        virtual void copy_from(const Buffer &other, u64 src_offset_count, u64 dst_offset_count, u64 size_count) = 0;
    };

    class IndexBuffer : public RuntimeComponent {
    public:
        virtual void set_count(u32 count) = 0;

        virtual void *map() = 0;
        virtual void unmap() = 0;
        virtual void flush(u32 offset_count, u32 count) = 0;
        virtual void write_data(const void *data, u32 offset_count, u32 count) = 0;
        virtual void resize(u32 count) = 0;
        virtual void copy_from(const Buffer &other, u64 src_offset_count, u64 dst_offset_count, u64 size_count) = 0;
    };
}
