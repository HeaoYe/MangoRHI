#pragma once

#include "vulkan_commons.hpp"
#include "command.hpp"

namespace MangoRHI {
    class VulkanCommand final : public Command {
    public:
        void set_single_use(Bool is_single_use);
        void set_command_buffer(VkCommandBuffer command_buffer);

        Result create() override;
        Result destroy() override;
        Result begin_render() override;
        Result end_render() override;

        void next_subpass() override;
        void bind_vertex_buffer(const VertexBuffer *vertex_buffer) override;
        void bind_index_buffer(const IndexBuffer *index_buffer) override;
        void draw_instances(u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) override;
        void draw_indexed_instances(u32 index_count, u32 instance_count, u32 first_index, u32 first_instance, u32 vertex_offset) override;
        void set_viewport(Viewport &viewport) override;
        void set_scissor(Scissor &scissor) override;
    
    define_member(VkCommandBuffer, command_buffer, VK_NULL_HANDLE)
    define_member(Bool, is_single_use, MG_FALSE)
    define_private_member(u32, _current_subpass, 0)

    no_copy_and_move_construction(VulkanCommand)
    };
}
