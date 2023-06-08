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
        virtual void add_vertex_attribute(VertexInputType type, u32 stride) = 0;
        virtual void add_vertex_binding(VertexInputRate rate) = 0;
        virtual void attach_vertex_shader(const Shader *shader, const char *entry) = 0;
        virtual void attach_fragment_shader(const Shader *shader, const char *entry) = 0;
        virtual void set_is_external_shader_program(Bool is_external_shader_program) = 0;
        virtual DescriptorSet *create_descriptor_set() = 0;
    };
}
