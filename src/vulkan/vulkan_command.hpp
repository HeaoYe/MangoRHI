#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/command.hpp"

namespace MangoRHI {
    class VulkanCommandPool;

    class VulkanCommand final : public Command {
        friend VulkanCommandPool;
    public:
        Result create() override;
        Result destroy() override;
        Result begin_render() override;
        Result end_render() override;

        void next_subpass() override;
        void bind_vertex_buffer(const VertexBuffer *vertex_buffer, u32 binding) override;
        void bind_index_buffer(const IndexBuffer *index_buffer) override;
        void draw_instances(u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) override;
        void draw_indexed_instances(u32 index_count, u32 instance_count, u32 first_index, u32 first_instance, u32 vertex_offset) override;
        void set_viewport(const Viewport &viewport) override;
        void set_scissor(const Scissor &scissor) override;
    
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, Bool, is_single_use, MG_FALSE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkCommandBuffer, command_buffer, VK_NULL_HANDLE)
    define_private_member(u32, _current_subpass, 0)

    declare_component_cls(VulkanCommand)
    };
}
