#pragma once

#include "../commons.hpp"

namespace MangoRHI {
    class RenderTarget : public RuntimeComponent {
    public:
        virtual void set_name(const char *name) = 0;
        virtual void set_clear_value(ClearValue clear_value) = 0;
        virtual void set_usage(RenderTargetUsage usage) = 0;
    };
}
