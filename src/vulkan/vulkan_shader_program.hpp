#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/shader_program.hpp"
#include "resource/vulkan_shader.hpp"
#include "vulkan_descriptor_set.hpp"

namespace MangoRHI {
    struct VulkanShaderInfo {
        VulkanShader *shader;
        const char *entry;
    };

    class VulkanShaderProgram final : public ShaderProgram {
    public:
        void set_topology(Topology topology) override;
        void set_polygon_mode(PolygonMode polygon) override;
        void set_front_face(FrontFace front) override;
        void set_cull_mode(CullMode cull) override;
        void add_vertex_attribute(VertexInputType type, u32 stride) override;
        void add_vertex_binding(VertexInputRate rate) override;
        void attach_vertex_shader(const Shader *shader, const char *entry) override;
        void attach_fragment_shader(const Shader *shader, const char *entry) override;
        DescriptorSet *create_descriptor_set() override;

        Result create() override;
        Result destroy() override;

    define_member(VkPipelineLayout, layout, VK_NULL_HANDLE)
    define_member(VkPipeline, pipeline, VK_NULL_HANDLE)
    define_member(STL_IMPL::vector<VkDescriptorSetLayout>, descriptor_set_layouts, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VkDescriptorSet>, descriptor_sets, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(u32, subpass_index, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanDescriptorSet *>, vulkan_descriptor_sets, MANGO_NO_INIT_VAULE)
    define_private_member(Topology, topology, Topology::eTriangleList)
    define_private_member(PolygonMode, polygon, PolygonMode::eFill)
    define_private_member(FrontFace, front, FrontFace::eClockwise)
    define_private_member(CullMode, cull, CullMode::eBack)
    define_private_member(STL_IMPL::vector<VkVertexInputAttributeDescription>, attributes, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkVertexInputBindingDescription>, bindings, MANGO_NO_INIT_VAULE)
    define_private_member(u32, _current_location, 0)
    define_private_member(u32, _current_offset, 0)
    define_private_member(VulkanShaderInfo, vertex_shader, MANGO_NO_INIT_VAULE)
    define_private_member(VulkanShaderInfo, fragment_shader, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkDynamicState>, dynamic_states, MANGO_NO_INIT_VAULE)

    no_copy_and_move_construction(VulkanShaderProgram)
    };
}
