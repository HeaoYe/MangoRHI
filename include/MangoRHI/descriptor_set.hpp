#pragma once

#include "commons.hpp"
#include "resource/texture.hpp"

namespace MangoRHI {
    class DescriptorSet : public RuntimeComponent {
    public:
        virtual void add_uniform(DescriptorStage stage, u32 size, u32 count) = 0;
        virtual void add_textures(DescriptorStage stage, Texture **textures, u32 count) = 0;
        virtual void *map_uniform_buffer_pointer(u32 binding, u32 index) = 0;
    };
}
