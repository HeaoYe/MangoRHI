#pragma once

#include "../commons.hpp"

namespace MangoRHI {
    class Texture;

    class Sampler : public RuntimeComponent {
    public:
        virtual void set_mag_filter(SamplerFilter filter) = 0;
        virtual void set_min_filter(SamplerFilter filter) = 0;
        virtual void set_address_mode(SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w) = 0;
        virtual void set_max_anisotropy(u32 anisotropy) = 0;
        virtual void set_border_color(SamplerBorderColor color) = 0;
        virtual void set_mipmap_mode(SamplerFilter mipmap) = 0;
    };
}
