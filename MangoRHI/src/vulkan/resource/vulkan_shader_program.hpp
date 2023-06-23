#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/shader_program.hpp"
#include "vulkan_shader.hpp"
#include "../vulkan_descriptor_set.hpp"

namespace MangoRHI {
    struct VulkanShaderInfo {
        VulkanShader *shader;
        const char *entry;
    };

    class VulkanShaderProgram final : public ShaderProgram {
    public:
        void add_dynamic_state(DynamicState state) override;
        void add_vertex_attribute(VertexInputType type, u32 stride) override;
        void add_vertex_binding(VertexInputRate rate) override;
        void attach_vertex_shader(const Shader &shader, const char *entry) override;
        void attach_fragment_shader(const Shader &shader, const char *entry) override;
        std::weak_ptr<DescriptorSetLayout> create_descriptor_set_layout(const char *layout_name) override;
        std::unique_ptr<DescriptorSet> allocate_descriptor_set(const char *layout_name) override;
        void free_descriptor_set(std::unique_ptr<DescriptorSet> descriptor_set) override;

    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, subpass_index, MANGO_NO_INIT_VAULE)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, Topology, VkPrimitiveTopology, topology, topology2vk_primitive_topology, Topology::eTriangleList)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, PolygonMode, VkPolygonMode, polygon_mode, polygon_mode2vk_polygon_mode, PolygonMode::eFill)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, FrontFace, VkFrontFace, front_face, front_face2vk_front_face, FrontFace::eClockwise)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, CullMode, VkCullModeFlags, cull_mode, cull_mode2vk_cull_mode_flags, CullMode::eBack)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, Bool, VkBool32, depth_test_enabled, bool2vk_bool32, MG_FALSE)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, Bool, VkBool32, depth_write_enabled, bool2vk_bool32, MG_TRUE)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, DepthCompareOp, VkCompareOp, depth_compare_op, depth_compare_op2vk_compare_op, DepthCompareOp::eLess)
    define_private_member(STL_IMPL::vector<VkVertexInputAttributeDescription>, attributes, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkVertexInputBindingDescription>, bindings, MANGO_NO_INIT_VAULE)
    define_private_member(VulkanShaderInfo, vertex_shader, MANGO_NO_INIT_VAULE)
    define_private_member(VulkanShaderInfo, fragment_shader, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkDynamicState>, dynamic_states, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<std::shared_ptr<VulkanDescriptorSetLayout>>, descriptor_set_layouts, MANGO_NO_INIT_VAULE)

    define_private_member(u32, _current_location, 0)
    define_private_member(u32, _current_offset, 0)

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkPipelineLayout, layout, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkPipeline, pipeline, VK_NULL_HANDLE)

    declare_component_cls(VulkanShaderProgram)
    };
}
