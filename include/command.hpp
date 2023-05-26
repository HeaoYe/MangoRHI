#pragma once

#include "commons.hpp"

namespace MangoRHI {
    class Command : public RuntimeComponent {
    public:
        virtual Result begin_render() = 0;
        virtual Result end_render() = 0;
        // virtual void set_viewport() = 0;
        // virtual void set_scissor() = 0;
    };
}
