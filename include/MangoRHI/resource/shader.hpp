#pragma once

#include "../commons.hpp"

namespace MangoRHI {
    class Shader : public RuntimeComponent {
    public:
        virtual void set_filename(const char *filename) = 0;
    };
}
