#pragma once

#include "commons.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "command.hpp"
#include "shader_program.hpp"
#include "resource_manager.hpp"

namespace MangoRHI {
    class Context : public RuntimeComponent {
    public:
        virtual void set_api_info(const void *info) = 0;
        virtual void set_device_name(const char *name) = 0;
        virtual void set_swapchain_image_count(u32 count) = 0;
        virtual void set_max_in_flight_frame_count(u32 count) = 0;
        virtual void set_multisample_count(MultisampleCount count) = 0;

        virtual void resize(u32 width, u32 height) = 0;
        virtual const u32 get_width() const = 0;
        virtual const u32 get_height() const = 0;
        virtual ResourceManager &get_resource_manager_reference() = 0;
        virtual RenderTarget &get_surface_render_target_reference() = 0;
        virtual RenderPass &get_render_pass_reference() = 0;
        virtual Command &get_current_command_reference() = 0;

        virtual Result begin_frame() = 0;
        virtual Result end_frame() = 0;
    };
}
