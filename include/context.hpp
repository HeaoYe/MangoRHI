#pragma once

#include "commons.hpp"
#include "swapchain.hpp"

namespace MangoRHI {
    class Context : public RuntimeComponent {
    public:
        virtual void set_api_info(const void *info) = 0;
        virtual void set_application_name(const char *name) = 0;
        virtual void set_device_name(const char *name) = 0;
        virtual void set_swapchain_image_count(u32 count) = 0;
        virtual void resize(const u32 width, const u32 height) = 0;
        virtual Result acquire_next_frame() {
            return get_swapchain()->acquire_next_frame();
        }

        virtual Result present() {
            return get_swapchain()->present();
        }
    protected:
        virtual Swapchain *get_swapchain() = 0;
    };
}
