#include "MangoRHI/MangoRHI.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto *glfwWindow = glfwCreateWindow(640, 640, "Sandbox", nullptr, nullptr);

    MangoRHI::set_logger_level(MangoRHI::LogLevel::eDebug);
    MangoRHI::initialize(MangoRHI::API::eVulkan);
    MangoRHI::Context *ctx = MangoRHI::get_context();

    #ifdef MANGO_DEBUG
    RHI_WARN("Debug Mode")
    #endif

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

    main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    main_shader_program->add_vertex_binding(MangoRHI::VertexInputRate::ePerVertex);
    main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    main_shader_program->add_vertex_binding(MangoRHI::VertexInputRate::ePerInstance);
    main_shader_program->attach_vertex_shader(ctx->create_shader("examples/Sandbox/assets/vert.spv"), "main");
    main_shader_program->attach_fragment_shader(ctx->create_shader("examples/Sandbox/assets/frag.spv"), "main");
    main_shader_program->set_cull_mode(MangoRHI::CullMode::eNone);
    auto *ds = main_shader_program->create_descriptor_set();
    ds->add_uniform(MangoRHI::DescriptorStage::eVertex, sizeof(float) * 2, 2);

    auto *vertex_buffer = ctx->create_vertex_buffer();
    auto *color_buffer = ctx->create_vertex_buffer();
    auto *index_buffer = ctx->create_index_buffer();
    vertex_buffer->set_vertex_size(sizeof(glm::vec3));
    color_buffer->set_vertex_size(sizeof(glm::vec3));
    
    ctx->create();

    float *uniform_buffer_pointer0 = (float *)ds->map_uniform_buffer_pointer(0, 0);
    float *uniform_buffer_pointer1 = (float *)ds->map_uniform_buffer_pointer(0, 1);
    const std::vector<glm::vec3> vertices = {
        { 0.5f, 0.5f, 0.0f },
        { 0.5f, -0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
        { -0.5f, 0.5f, 0.0f },
    };
    const std::vector<glm::vec3> colors = {
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 1.0f },
    };
    const std::vector<MangoRHI::u32> indices = {
        0, 1, 2, 
        0, 3, 2,
    };
    vertex_buffer->write_data(vertices.data(), vertices.size(), 0);
    color_buffer->write_data(colors.data(), colors.size(), 0);
    index_buffer->write_data(indices.data(), indices.size(), 0);

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if (ctx->begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = ctx->get_current_command_reference();

            command.next_subpass();
            static float t = 0.0f;
            t += 0.01f;
            *(uniform_buffer_pointer0 + 0) = (glm::sin(t) + 0.2f) * 0.5f;
            *(uniform_buffer_pointer0 + 1) = t * 1.5f;
            *(uniform_buffer_pointer1 + 0) = glm::abs((glm::sin(t) + 1.2f) * 0.3f);
            *(uniform_buffer_pointer1 + 1) = -t * 2.0f;
            ctx->set_clear_color(MangoRHI::ColorClearValue { .r = (glm::sin(t) + 1.0f) / 2.0f, .g = (glm::sin(t + 3.14159265358979f * 2.0f / 3.0f) + 1.0f) / 2.0f, .b = (glm::sin(t + 3.14159265358979f * 4.0f / 3.0f) + 1.0f) / 2.0f, .a = 1.0f } );
            auto viewport = MangoRHI::Viewport { 0, 0, static_cast<float>(ctx->get_width()), static_cast<float>(ctx->get_height()), 0.0f, 1.0f };
            auto scissor = MangoRHI::Scissor { 0, 0, ctx->get_width(), ctx->get_height() };
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.bind_vertex_buffer(vertex_buffer, 0);
            command.bind_vertex_buffer(color_buffer, 1);
            command.bind_index_buffer(index_buffer);
            command.draw_indexed_instances(6, 4, 0, 0,  0);

            ctx->end_frame();
        }
    }

    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
