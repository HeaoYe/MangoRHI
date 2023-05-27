#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/buffer.hpp"

namespace MangoRHI {
    class VulkanBuffer final : public Buffer {
    public:
        void set_size(u32 size) override;

        Result create() override;
        Result destroy() override;

        void *map();
        void unmap();
        void write_data(const void *data, u32 size, u32 offset) override;
        void resize(u32 size) override;
        void copy_from(const Buffer *other, u64 src_offset, u64 dst_offset, u64 size) override;
    
    define_member(u32, size, MANGO_NO_INIT_VAULE)
    define_member(VkBuffer, buffer, VK_NULL_HANDLE)
    define_member(VkDeviceMemory, memory, VK_NULL_HANDLE)
    define_extern_writeable_member(VkBufferUsageFlags, usage, 0)
    define_extern_writeable_member(VkMemoryPropertyFlags, properties, 0)

    no_copy_and_move_construction(VulkanBuffer)
    };

    class VulkanVertexBuffer final : public VertexBuffer {
    public:
        void set_vertex_size(u32 size) override;
        void set_size(u32 count) override;
        
        Result create() override;
        Result destroy() override;
        void write_data(const void *data, u32 count, u32 offset_count) override;
        void resize(u32 size) override;
        void copy_from(const Buffer *other, u64 src_offset_count, u64 dst_offset_count, u64 count) override;
    
    define_member(u32, size, MANGO_NO_INIT_VAULE)
    define_private_member(u32, type_size, MANGO_NO_INIT_VAULE)
    define_member(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(VulkanBuffer, buffer, MANGO_NO_INIT_VAULE)

    no_copy_and_move_construction(VulkanVertexBuffer)
    };

    class VulkanIndexBuffer final : public IndexBuffer {
    public:
        void set_size(u32 count) override;
        
        Result create() override;
        Result destroy() override;
        void write_data(const void *data, u32 count, u32 offset_count) override;
        void resize(u32 count) override;
        void copy_from(const Buffer *other, u64 src_offset_count, u64 dst_offset_count, u64 count) override;
    
    define_member(u32, size, MANGO_NO_INIT_VAULE)
    define_private_member(u32, type_size, sizeof(u32))
    define_member(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(VulkanBuffer, buffer, MANGO_NO_INIT_VAULE)

    no_copy_and_move_construction(VulkanIndexBuffer)
    };
}
