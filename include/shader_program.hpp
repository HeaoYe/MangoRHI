#pragma once

#include "commons.hpp"
#include "resource/shader.hpp"

namespace MangoRHI {
    class ShaderProgram : public RuntimeComponent {
    public:
        virtual void bind_subpass(const char *subpass_name) = 0;
        virtual void set_topology(Topology topology) = 0;
        virtual void set_polygon_mode(PolygonMode polygon) = 0;
        virtual void set_front_face(FrontFace front) = 0;
        virtual void set_cull_mode(CullMode cull) = 0;
        virtual void add_vertex_attribute(VertexInputType type, u32 stride) = 0;
        virtual void add_vertex_binding(VertexInputRate rate) = 0;
        virtual void attach_vertex_shader(Shader *shader, const char *entry) = 0;
        virtual void attach_fragment_shader(Shader *shader, const char *entry) = 0;
    };
}
