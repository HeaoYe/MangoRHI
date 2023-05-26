#pragma once

#include "commons.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"

namespace MangoRHI {
    class Context : public RuntimeComponent {
    public:
        virtual void set_api_info(const void *info) = 0;
        virtual void set_application_name(const char *name) = 0;
        virtual void set_device_name(const char *name) = 0;
        virtual void set_swapchain_image_count(const u32 count) = 0;
        virtual void set_max_in_flight_image_count(const u32 count) = 0;
        virtual void set_clear_color(ColorClearValue clear_color) = 0;
        virtual void resize(const u32 width, const u32 height) = 0;
        virtual Result acquire_next_frame() {
            return swapchain->acquire_next_frame();
        }

        virtual Result present() {
            return swapchain->present();
        }
    protected:
    define_pointer(Swapchain, swapchain, MANGO_NO_INIT_VAULE)
    define_extern_writeable_pointer(RenderPass, render_pass, MANGO_NO_INIT_VAULE)
    };
}
