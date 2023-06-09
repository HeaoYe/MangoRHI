#pragma once

#include "../commons.hpp"
#include "shader.hpp"
#include "../descriptor_set.hpp"

namespace MangoRHI {
    class ShaderProgram : public RuntimeComponent {
    public:
        virtual void set_topology(Topology topology) = 0;
        virtual void set_polygon_mode(PolygonMode polygon) = 0;
        virtual void set_front_face(FrontFace front) = 0;
        virtual void set_cull_mode(CullMode cull) = 0;
        virtual void set_depth_test_enabled(Bool enabled) = 0;
        virtual void set_depth_write_enabled(Bool enabled) = 0;
        virtual void set_depth_compare_op(DepthCompareOp op) = 0;
        virtual void add_dynamic_state(DynamicState state) = 0;
        virtual void add_vertex_attribute(VertexInputType type, u32 stride) = 0;
        virtual void add_vertex_binding(VertexInputRate rate) = 0;
        virtual void attach_vertex_shader(const Shader &shader, const char *entry) = 0;
        virtual void attach_fragment_shader(const Shader &shader, const char *entry) = 0;
        virtual std::weak_ptr<DescriptorSetLayout> create_descriptor_set_layout(const char *layout_name) = 0;
        virtual std::unique_ptr<DescriptorSet> allocate_descriptor_set(const char *layout_name) = 0;
        virtual void free_descriptor_set(std::unique_ptr<DescriptorSet> descriptor_set) = 0;
    };
}
