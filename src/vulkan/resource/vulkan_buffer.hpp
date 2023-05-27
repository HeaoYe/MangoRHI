#pragma once

#include "../vulkan_commons.hpp"
#include "resource/buffer.hpp"

namespace MangoRHI {
    class VulkanBuffer final : public Buffer {
    public:
        void set_size(const u32 size) override;

        Result create() override;
        Result destroy() override;

        void *map();
        void unmap();
        void write_data(const void *data, const u32 size, const u32 offset) override;
        void resize(const u32 size) override;
        void copy_from(const Buffer *other, const u64 src_offset, const u64 dst_offset, const u64 size) override;
    
    define_member(u32, size, MANGO_NO_INIT_VAULE)
    define_member(VkBuffer, buffer, VK_NULL_HANDLE)
    define_member(VkDeviceMemory, memory, VK_NULL_HANDLE)
    define_extern_writeable_member(VkBufferUsageFlags, usage, 0)
    define_extern_writeable_member(VkMemoryPropertyFlags, properties, 0)
    };

    class VulkanVertexBuffer final : public VertexBuffer {
    public:
        void set_vertex_size(const u32 size) override;
        void set_size(const u32 count) override;
        
        Result create() override;
        Result destroy() override;
        void write_data(const void *data, const u32 count, const u32 offset_count) override;
        void resize(const u32 size) override;
        void copy_from(const Buffer *other, const u64 src_offset_count, const u64 dst_offset_count, const u64 count) override;
    
    define_member(u32, size, MANGO_NO_INIT_VAULE)
    define_private_member(u32, type_size, MANGO_NO_INIT_VAULE)
    define_member(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(VulkanBuffer, buffer, MANGO_NO_INIT_VAULE)
    };

    class VulkanIndexBuffer final : public IndexBuffer {
    public:
        void set_size(const u32 count) override;
        
        Result create() override;
        Result destroy() override;
        void write_data(const void *data, const u32 count, const u32 offset_count) override;
        void resize(const u32 count) override;
        void copy_from(const Buffer *other, const u64 src_offset_count, const u64 dst_offset_count, const u64 count) override;
    
    define_member(u32, size, MANGO_NO_INIT_VAULE)
    define_private_member(u32, type_size, sizeof(u32))
    define_member(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(VulkanBuffer, buffer, MANGO_NO_INIT_VAULE)
    };
}
