#pragma once

#include "../commons.hpp"
#include "sampler.hpp"

namespace MangoRHI {
    class Texture : public RuntimeComponent {
    public:
        virtual void set_filename(const char *filename) = 0;
        virtual void bind_sampler(Sampler *sampler) = 0;
    };
}
