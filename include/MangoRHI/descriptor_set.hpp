#pragma once

#include "commons.hpp"
#include "resource/texture.hpp"

namespace MangoRHI {
    class DescriptorSet : public RuntimeComponent {
    public:
        virtual u32 add_uniforms(DescriptorStage stage, u32 size, u32 count) = 0;
        virtual u32 add_textures(DescriptorStage stage, Texture **textures, u32 count) = 0;
        virtual u32 add_input_render_targets(DescriptorStage stage, const char **render_target_names, Sampler **samplers, u32 count) = 0;
        virtual void *get_uniform_buffer_pointer(u32 binding, u32 index) = 0;
        virtual void set_texture(u32 binding, u32 index, Texture *texture) = 0;
        virtual void set_input_render_target(u32 binding, u32 index, const char *render_target_name, Sampler *samplers) = 0;
        virtual void update() = 0;
    };
}
