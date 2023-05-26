#pragma once

#include "commons.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "command.hpp"

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
        virtual RenderPass &get_render_pass_reference() = 0;
        virtual Command &get_current_command() = 0;

        virtual Result begin_frame() = 0;
        virtual Result end_frame() = 0;
    };
}
