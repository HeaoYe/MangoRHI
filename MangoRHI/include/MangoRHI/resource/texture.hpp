#pragma once

#include "../commons.hpp"

namespace MangoRHI {
    class Sampler;

    class Texture : public RuntimeComponent {
    public:
        virtual void set_filename(const char *filename) = 0;
        virtual void set_is_empty(Bool is_empty) = 0;
        virtual void set_mipmap_levels(u32 levels) = 0;
        virtual Sampler &get_sampler_reference() = 0;
    };
}
