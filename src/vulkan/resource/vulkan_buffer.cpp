#include "vulkan_buffer.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanBuffer::set_size(u32 size) {
        this->size = size;
    }

    Result VulkanBuffer::create() {
        component_create()

        VkBufferCreateInfo buffer_create_info { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        buffer_create_info.pQueueFamilyIndices = &vulkan_context->get_device().get_graphics_family_index();
        buffer_create_info.queueFamilyIndexCount = 1;
        buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_create_info.usage = usage;
        buffer_create_info.size = size;
        VK_CHECK(vkCreateBuffer(vulkan_context->get_device().get_logical_device(), &buffer_create_info, vulkan_context->get_allocator(), &buffer))
        RHI_DEBUG("Create vulkan buffer -> 0x{:x}", (AddrType)buffer)

        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(vulkan_context->get_device().get_logical_device(), buffer, &requirements);
        VkMemoryAllocateInfo allocateInfo { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocateInfo.allocationSize = requirements.size;
        allocateInfo.memoryTypeIndex = vulkan_context->find_memory_index(requirements.memoryTypeBits, properties);
        VK_CHECK(vkAllocateMemory(vulkan_context->get_device().get_logical_device(), &allocateInfo, vulkan_context->get_allocator(), &memory));
        RHI_DEBUG("Allocate vulkan device memory -> 0x{:x}", (AddrType)memory)

        vkBindBufferMemory(vulkan_context->get_device().get_logical_device(), buffer, memory, 0);

        return Result::eSuccess;
    }

    Result VulkanBuffer::destroy() {
        component_destroy()

        RHI_DEBUG("Free vulkan device memory -> 0x{:x}", (AddrType)memory)
        vkFreeMemory(vulkan_context->get_device().get_logical_device(), memory, vulkan_context->get_allocator());

        RHI_DEBUG("Destroy vulkan buffer -> 0x{:x}", (AddrType)buffer)
        vkDestroyBuffer(vulkan_context->get_device().get_logical_device(), buffer, vulkan_context->get_allocator());

        return Result::eSuccess;
    }

    void *VulkanBuffer::map() {
        void *data;
        vkMapMemory(vulkan_context->get_device().get_logical_device(), memory, 0, size, 0, &data);
        return data;
    }

    void VulkanBuffer::unmap() {
        vkUnmapMemory(vulkan_context->get_device().get_logical_device(), memory);
    }

    void VulkanBuffer::write_data(const void *data, const u32 size, const u32 offset) {
        void *buffer_ptr = (void *)(((u8 *)map()) + offset);
        memcpy(buffer_ptr, data, size);
        unmap();
    }

    void VulkanBuffer::resize(const u32 size) {
        VulkanBuffer buffer {};
        buffer.get_usage() = usage;
        buffer.get_properties() = properties;
        buffer.set_size(size);
        buffer.create();
        buffer.copy_from(this, 0, 0, size);
        destroy();
        this->buffer = buffer.buffer;
        this->memory = buffer.memory;
        this->size = buffer.size;
        component_create();
    }

    void VulkanBuffer::copy_from(const Buffer *other, const u64 src_offset, const u64 dst_offset, const u64 size) {
        VulkanCommand command;
        vulkan_context->get_command_pool().allocate_single_use(&command);
        VkBufferCopy copy;
        copy.srcOffset = src_offset;
        copy.dstOffset = dst_offset;
        copy.size = size;
        vkCmdCopyBuffer(command.get_command_buffer(), ((VulkanBuffer *)other)->buffer, buffer, 1, &copy);
        vulkan_context->get_command_pool().free(&command);
    }

    void VulkanVertexBuffer::set_vertex_size(const u32 size) {
        this->type_size = size;
    }

    void VulkanVertexBuffer::set_size(const u32 count) {
        this->size = count * type_size;
    }

    Result VulkanVertexBuffer::create() {
        component_create()

        staging.set_size(size);
        staging.get_usage() = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging.get_properties() = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        buffer.set_size(size);
        buffer.get_usage() = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        buffer.get_properties() = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        staging.create();
        buffer.create();

        return Result::eSuccess;
    }

    Result VulkanVertexBuffer::destroy() {
        component_destroy()

        staging.destroy();
        buffer.destroy();

        return Result::eSuccess;
    }

    void VulkanVertexBuffer::write_data(const void *data, const u32 count, const u32 offset_count) {
        u32 size = count * type_size;
        staging.write_data(data, size, 0);
        buffer.copy_from(&staging, 0, offset_count * type_size, size);
    }

    void VulkanVertexBuffer::resize(const u32 count) {
        staging.resize(count * type_size);
        buffer.resize(count * type_size);
    }

    void VulkanVertexBuffer::copy_from(const Buffer *other, const u64 src_offset_count, const u64 dst_offset_count, const u64 count) {
        buffer.copy_from(&((const VulkanVertexBuffer *)other)->get_buffer(), src_offset_count * type_size, dst_offset_count * type_size, count * type_size);
    }

    void VulkanIndexBuffer::set_size(const u32 count) {
        this->size = count * type_size;
    }

    Result VulkanIndexBuffer::create() {
        component_create()

        staging.set_size(size);
        staging.get_usage() = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging.get_properties() = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        buffer.set_size(size);
        buffer.get_usage() = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        buffer.get_properties() = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        staging.create();
        buffer.create();

        return Result::eSuccess;
    }

    Result VulkanIndexBuffer::destroy() {
        component_destroy()

        staging.destroy();
        buffer.destroy();

        return Result::eSuccess;
    }

    void VulkanIndexBuffer::write_data(const void *data, const u32 count, const u32 offset_count) {
        u32 size = count * type_size;
        staging.write_data(data, size, 0);
        buffer.copy_from(&staging, 0, offset_count * type_size, size);
    }

    void VulkanIndexBuffer::resize(const u32 count) {
        staging.resize(count * type_size);
        buffer.resize(count * type_size);
    }

    void VulkanIndexBuffer::copy_from(const Buffer *other, const u64 src_offset_count, const u64 dst_offset_count, const u64 count) {
        buffer.copy_from(&((const VulkanIndexBuffer *)other)->get_buffer(), src_offset_count * type_size, dst_offset_count * type_size, count * type_size);
    }
}
