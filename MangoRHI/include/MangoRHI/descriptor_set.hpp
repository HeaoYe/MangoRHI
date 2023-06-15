#pragma once

#include "commons.hpp"
#include "resource/texture.hpp"

namespace MangoRHI {
    class DescriptorSetLayout : public RuntimeComponent {
    public:
        virtual void set_name(const char *name) = 0;
        virtual u32 add_uniforms_descriptor(DescriptorStageFlags stages, u32 size, u32 count) = 0;
        virtual u32 add_textures_descriptor(DescriptorStageFlags stages, u32 count) = 0;
        virtual u32 add_input_render_targets_descriptor(DescriptorStageFlags stages, u32 count) = 0;
    };

    class DescriptorSet : public RuntimeComponent {
    public:
        virtual void *get_uniform_buffer_pointer(u32 binding, u32 index) = 0;
        virtual void set_texture(u32 binding, u32 index, Texture &texture) = 0;
        virtual void set_input_render_target(u32 binding, u32 index, const STL_IMPL::pair<const char *, const Sampler &> &render_target) = 0;

        virtual void update(u32 binding) = 0;
        virtual void update_all() = 0;
    };
}
