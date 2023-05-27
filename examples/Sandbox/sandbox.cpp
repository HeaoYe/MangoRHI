#include "MangoRHI.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 pos;
    // glm::vec3 color;
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto *glfwWindow = glfwCreateWindow(640, 640, "Sandbox", nullptr, nullptr);

    MangoRHI::set_logger_level(MangoRHI::LogLevel::eDebug);
    MangoRHI::initialize(MangoRHI::API::eVulkan);
    MangoRHI::Context *ctx = MangoRHI::get_context();

    MangoRHI::VulkanContextInfo info;
    info.extensions = glfwGetRequiredInstanceExtensions(&info.extension_count);
    info.surface_create_callback = [glfwWindow](VkInstance &instance, VkAllocationCallbacks *allocator) {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        glfwCreateWindowSurface(instance, glfwWindow, allocator, &surface);
        return surface;
    };
    info.app_name = "MangoRHI Sanbox App";
    info.engine_name = "No Engine";
    ctx->set_api_info(&info);
    ctx->set_device_name("NVIDIA GeForce RTX 4090");
    ctx->set_swapchain_image_count(3);
    ctx->set_max_in_flight_frame_count(2);

    ctx->set_clear_color(MangoRHI::ColorClearValue { .r = 0.05f, .g = 0.1f, .b = 0.08f, .a = 1.0f } );
    auto &rp = ctx->get_render_pass_reference();
    rp.add_output_render_target(MANGORHI_SURFACE_RENDER_TARGET_NAME, MangoRHI::RenderTargetLayout::eColor);
    auto *main_shader_program = rp.add_subpass("main", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_dependency({ MANGORHI_EXTERNAL_SUBPASS_NAME, MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eNone }, { "main", MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eColorRenderTargetWrite });

    main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, 12);
    // main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, 12);
    main_shader_program->add_vertex_binding(MangoRHI::VertexInputRate::ePerVertex);
    main_shader_program->attach_vertex_shader(ctx->create_shader("examples/Sandbox/assets/vert.spv"), "main");
    main_shader_program->attach_fragment_shader(ctx->create_shader("examples/Sandbox/assets/frag.spv"), "main");
    main_shader_program->set_cull_mode(MangoRHI::CullMode::eNone);

    auto &vertex_buffer = ctx->get_vertex_buffer_reference();
    auto &index_buffer = ctx->get_index_buffer_reference();
    vertex_buffer.set_vertex_size(sizeof(Vertex));
    
    ctx->create();

    // Vertex vertices[] = {
    //     { { 1, 1, 0 },   { 0, 1, 0 } },
    //     { { -1, 1, 0 },  { 0, 1, 1 } },
    //     { { -1, -1, 0 }, { 1, 0, 1 } },
    //     { { 1, -1, 0 },  { 1, 0, 0 } },
    // };
    Vertex vertices[] = {
        { { 1, 1, 0 } },
        { { -1, 1, 0 } },
        { { -1, -1, 0 } },
        { { 1, -1, 0 } },
    };
    MangoRHI::u32 indices[] = {
        0, 1, 2, 
        0, 3, 2,
    };
    vertex_buffer.write_data(vertices, 4, 0);
    index_buffer.write_data(indices, 6, 0);

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if (ctx->begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = ctx->get_current_command_reference();

            command.next_subpass();
            auto viewport = MangoRHI::Viewport { 0, 0, static_cast<float>(ctx->get_width()), static_cast<float>(ctx->get_height()), 0.0f, 1.0f };
            auto scissor = MangoRHI::Scissor { 0, 0, ctx->get_width(), ctx->get_height() };
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.bind_vertex_buffer(&vertex_buffer);
            command.bind_index_buffer(&index_buffer);
            command.draw_indexed_instances(3, 1, 0, 0, 0);
            // command.draw_indexed_instances(6, 1, 0, 0,  0);

            ctx->end_frame();
        }
    }

    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
