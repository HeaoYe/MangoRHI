#pragma once

#include "commons.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "command.hpp"
#include "shader_program.hpp"
#include "resource/buffer.hpp"

namespace MangoRHI {
    class Context : public RuntimeComponent {
    public:
        virtual void set_api_info(const void *info) = 0;
        virtual void set_device_name(const char *name) = 0;
        virtual void set_swapchain_image_count(const u32 count) = 0;
        virtual void set_max_in_flight_frame_count(const u32 count) = 0;
        virtual void set_clear_color(ColorClearValue clear_color) = 0;
        virtual Shader *create_shader(const char *filename) = 0;
        virtual void resize(const u32 width, const u32 height) = 0;
        virtual RenderPass &get_render_pass_reference() = 0;
        virtual Command &get_current_command_reference() = 0;

        virtual VertexBuffer &get_vertex_buffer_reference() = 0;
        virtual IndexBuffer &get_index_buffer_reference() = 0;

        virtual Result begin_frame() = 0;
        virtual Result end_frame() = 0;
    };
}
