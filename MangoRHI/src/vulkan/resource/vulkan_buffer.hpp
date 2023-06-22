#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/buffer.hpp"

namespace MangoRHI {
    class VulkanBuffer final : public Buffer {
    public:
        void *map() override;
        void unmap() override;
        void write_data(const void *data, u32 size, u32 offset) override;
        void resize(u32 size) override;
        void copy_from(const Buffer &other, u64 src_offset, u64 dst_offset, u64 size) override;

    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC, VkBufferUsageFlags, usage, 0)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC, VkMemoryPropertyFlags, properties, 0)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC_OVERRIDE, u32, size, MANGO_NO_INIT_VAULE)

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkBuffer, buffer, VK_NULL_HANDLE)
    define_private_member(VkDeviceMemory, memory, VK_NULL_HANDLE)
    define_private_pointer(void, ptr, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanBuffer)
    };

    class VulkanVertexBuffer final : public VertexBuffer {
    public:
        void *map() override;
        void unmap() override;
        void flush(u32 offset_count, u32 count) override;
        void write_data(const void *data, u32 count, u32 offset_count) override;
        void resize(u32 size) override;
        void copy_from(const Buffer &other, u64 src_offset_count, u64 dst_offset_count, u64 count) override;

    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC_OVERRIDE, u32, count, MANGO_NO_INIT_VAULE)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC_OVERRIDE, u32, vertex_size, MANGO_NO_INIT_VAULE)

    define_private_member(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanBuffer, buffer, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanVertexBuffer)
    };

    const u32 vulkan_index_type_size = sizeof(u32);

    class VulkanIndexBuffer final : public IndexBuffer {
    public:
        void *map() override;
        void unmap() override;
        void flush(u32 offset_count, u32 count) override;
        void write_data(const void *data, u32 count, u32 offset_count) override;
        void resize(u32 count) override;
        void copy_from(const Buffer &other, u64 src_offset_count, u64 dst_offset_count, u64 count) override;

    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC_OVERRIDE, u32, count, MANGO_NO_INIT_VAULE)

    define_private_member(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanBuffer, buffer, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanIndexBuffer)
    };
}
