#pragma once

#include "commons.hpp"
#include "resource/render_target.hpp"
#include "shader_program.hpp"

namespace MangoRHI {
    class RenderPass : public RuntimeComponent {
    public:
        virtual void attach_render_target(RenderTarget *render_target) = 0;

        virtual void add_input_render_target(const char *render_target_name, RenderTargetLayout ref_layout) = 0;
        virtual void add_output_render_target(const char *render_target_name, RenderTargetLayout ref_layout) = 0;
        virtual void add_preserve_render_target(const char *render_target_name) = 0;
        virtual void set_depth_render_target(const char *render_target_name, RenderTargetLayout ref_layout) = 0;
        virtual void set_resolve_render_target(const char *render_target_name, RenderTargetLayout ref_layout) = 0;
        virtual ShaderProgram *add_subpass(const char *subpass_name, PipelineBindPoint bind_point) = 0;
        virtual void add_dependency(SubpassStageInfo src_subpass_name, SubpassStageInfo dst_subpass_name) = 0;
    };
}
