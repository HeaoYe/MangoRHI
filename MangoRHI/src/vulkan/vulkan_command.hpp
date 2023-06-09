#pragma once

#include "vulkan_commons.hpp"
#include "resource/vulkan_shader_program.hpp"
#include "MangoRHI/command.hpp"

namespace MangoRHI {
    class VulkanCommandPool;

    class VulkanCommand final : public Command {
        friend VulkanCommandPool;
    public:
        Result begin_render() override;
        Result end_render() override;

        void next_subpass() override;
        void bind_shader_program(const ShaderProgram &shader_program) override;
        void bind_descriptor_set(const DescriptorSet &descriptor_set) override;
        void bind_vertex_buffer(const VertexBuffer &vertex_buffer, u32 binding) override;
        void bind_index_buffer(const IndexBuffer &index_buffer) override;
        void draw_instances(u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) override;
        void draw_indexed_instances(u32 index_count, u32 instance_count, u32 first_index, u32 first_instance, u32 vertex_offset) override;
        void set_viewport(const Viewport &viewport) override;
        void set_scissor(const Scissor &scissor) override;
        void set_depth_test_enabled(Bool enabled) override;
        void set_depth_write_enabled(Bool enabled) override;

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, Bool, is_single_use, MG_FALSE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkCommandBuffer, command_buffer, VK_NULL_HANDLE)
    define_private_pointer(VulkanShaderProgram, _current_shader_program, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanCommand)
    };
}
