#pragma once

#include "commons.hpp"

namespace MangoRHI {
    class Context : public RuntimeComponent {
    public:
        virtual void set_api_info(const void *info) = 0;
        virtual void set_application_name(const char *name) = 0;
        virtual void set_device_name(const char *name) = 0;
        virtual void resize(const u32 width, const u32 height) = 0;
    };
}
