#pragma once

#include "commons.hpp"

namespace MangoRHI {
    class Context : public RuntimeComponent {
    public:
        virtual void set_application_name(const char *name) = 0;
        virtual void set_device_name(const char *name) = 0;
        virtual void resize(u32 width, u32 height) = 0;
    };
}
