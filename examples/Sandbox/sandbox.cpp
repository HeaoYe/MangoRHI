#include "MangoRHI.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
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

    ctx->set_clear_color(MangoRHI::ColorClearValue { .r = 0.05f, .g = 0.1f, .b = 0.08f, .a = 1.0f } );
    auto &rp = ctx->get_render_pass_reference();
    rp.add_output_render_target(MANGORHI_SURFACE_RENDER_TARGET_NAME, MangoRHI::RenderTargetLayout::eColor);
    auto *main_shader_program = rp.add_subpass("main", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_dependency({ MANGORHI_EXTERNAL_SUBPASS_NAME, MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eNone }, { "main", MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eColorRenderTargetWrite });

    // main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(float) * 3);
    // main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(float) * 3);
    // main_shader_program->add_vertex_binding(MangoRHI::VertexInputRate::ePerVertex);
    main_shader_program->attach_vertex_shader(ctx->create_shader("examples/Sandbox/assets/vert.spv"), "main");
    main_shader_program->attach_fragment_shader(ctx->create_shader("examples/Sandbox/assets/frag.spv"), "main");
    main_shader_program->set_cull_mode(MangoRHI::CullMode::eNone);

    auto &vertex_buffer = ctx->get_vertex_buffer_reference();
    auto &index_buffer = ctx->get_index_buffer_reference();
    vertex_buffer.set_vertex_size(sizeof(Vertex));
    
    ctx->create();

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if (ctx->begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = (MangoRHI::VulkanCommand &)ctx->get_current_command_reference();

            // TODO: Record Command
            command.next_subpass();
            vkCmdDraw(command.get_command_buffer(), 3, 2, 0, 0);

            ctx->end_frame();
        }
    }

    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
