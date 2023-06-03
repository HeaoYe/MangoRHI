#include "MangoRHI/MangoRHI.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto *glfwWindow = glfwCreateWindow(640, 640, "Sandbox", nullptr, nullptr);
    vk::PipelineStageFlags a;

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

    auto *depth = ctx->create_render_target();
    depth->set_name("depth");
    depth->set_usage(MangoRHI::RenderTargetUsage::eDepth);
    depth->set_clear_color(MangoRHI::ClearValue { .depth_stencil = { .depth = 1.0f, .stencil = 0 } });
    
    auto &rp = ctx->get_render_pass_reference();
    rp.attach_render_target(depth);
    rp.add_output_render_target(MANGORHI_SURFACE_RENDER_TARGET_NAME, MangoRHI::RenderTargetLayout::eColor);
    rp.set_depth_render_target("depth", MangoRHI::RenderTargetLayout::eDepth);
    auto *main_shader_program = rp.add_subpass("main", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_dependency({ MANGORHI_EXTERNAL_SUBPASS_NAME, MangoRHI::PipelineStage::eColorOutput | MangoRHI::PipelineStage::eEarlyFragmentTest, MangoRHI::Access::eNone }, { "main", MangoRHI::PipelineStage::eColorOutput | MangoRHI::PipelineStage::eEarlyFragmentTest, MangoRHI::Access::eColorRenderTargetWrite | MangoRHI::Access::eDepthStencilRenderTargetWrite });

    auto *t_61 = ctx->create_texture();
    auto *t_paper_plane= ctx->create_texture();
    auto *t_dance = ctx->create_texture();
    auto *t_dhl = ctx->create_texture();
    t_61->set_filename("examples/Sandbox/assets/textures/61.png");
    t_paper_plane->set_filename("examples/Sandbox/assets/textures/paper plane.png");
    t_dance->set_filename("examples/Sandbox/assets/textures/dance.png");
    t_dhl->set_filename("examples/Sandbox/assets/textures/dhl.png");
    MangoRHI::Texture* textures[] = { t_61, t_paper_plane, t_dance, t_dhl };

    main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    main_shader_program->add_vertex_binding(MangoRHI::VertexInputRate::ePerVertex);
    main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    main_shader_program->add_vertex_binding(MangoRHI::VertexInputRate::ePerInstance);
    main_shader_program->attach_vertex_shader(ctx->create_shader("examples/Sandbox/assets/shaders/vert.spv"), "main");
    main_shader_program->attach_fragment_shader(ctx->create_shader("examples/Sandbox/assets/shaders/frag.spv"), "main");
    main_shader_program->set_cull_mode(MangoRHI::CullMode::eNone);
    main_shader_program->set_depth_test_enabled(MangoRHI::MG_TRUE);
    main_shader_program->set_depth_compare_op(MangoRHI::DepthCompareOp::eLessOrEqual);
    auto *ds = main_shader_program->create_descriptor_set();
    MangoRHI::u32 uniform_binding = ds->add_uniform(MangoRHI::DescriptorStage::eVertex, sizeof(float) * 2, 2);
    MangoRHI::u32 textures_binding = ds->add_textures(MangoRHI::DescriptorStage::eFragment, textures, 4);

    auto *vertex_buffer = ctx->create_vertex_buffer();
    auto *color_buffer = ctx->create_vertex_buffer();
    auto *index_buffer = ctx->create_index_buffer();
    vertex_buffer->set_vertex_size(sizeof(glm::vec3));
    color_buffer->set_vertex_size(sizeof(glm::vec3));
    
    ctx->create();

    struct UserPointer {
        MangoRHI::DescriptorSet * ds;
        MangoRHI::Texture **textures;
        MangoRHI::u32 textures_binding;
    } up { ds, textures, textures_binding };
    glfwSetWindowUserPointer(glfwWindow, &up);
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow *window, int key, int, int pressed, int) {
        if (pressed != 1) {
            return;
        }
        auto *up = (UserPointer *)glfwGetWindowUserPointer(window);
        int idx;
        switch (key) {
        case 49:
            idx = 0;
            break;
        case 50:
            idx = 1;
            break;
        case 51:
            idx = 2;
            break;
        case 52:
            idx = 3;
            break;
        case 53:
            idx = 4;
            break;
        default:
            return;
        }
        for (int i = 0; i < 4; i++) {
            up->ds->set_texture(up->textures_binding, i, up->textures[idx == 4 ? i : idx]);
        }
        up->ds->update();
    });
    std::vector<glm::vec3> vertices = {
        { 0.5f, 0.5f, 0.1f },
        { 0.5f, -0.5f, 0.2f },
        { -0.5f, -0.5f, 0.3f },
        { -0.5f, 0.5f, 0.4f },
    };
    std::vector<glm::vec3> colors = {
        { 245.0f, 245.0f, 245.0f },
        { 255.0f, 245.0f, 238.0f },
        { 255.0f, 250.0f, 240.0f },
        { 255.0f, 255.0f, 240.0f },
        { 253.0f, 245.0f, 230.0f },
        { 245.0f, 255.0f, 250.0f },
        { 240.0f, 255.0f, 255.0f },
        { 240.0f, 248.0f, 255.0f },
    };
    std::vector<MangoRHI::u32> indices = {
        0, 1, 2, 
        0, 3, 2,
    };
    for (auto &color : colors) {
        color = (color - 127.5f) / 127.5f;
    }
    vertex_buffer->write_data(vertices.data(), vertices.size(), 0);
    color_buffer->write_data(colors.data(), colors.size(), 0);
    index_buffer->write_data(indices.data(), indices.size(), 0);

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if (ctx->begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = ctx->get_current_command_reference();
            command.next_subpass();

            float *uniform_buffer_pointer0 = (float *)ds->get_uniform_buffer_pointer(uniform_binding, 0);
            float *uniform_buffer_pointer1 = (float *)ds->get_uniform_buffer_pointer(uniform_binding, 1);
            static float t1 = 0.0f, t2 = 0.0f;
            t1 += 0.01f;
            *(uniform_buffer_pointer0 + 0) = (glm::sin(t1) + 0.2f) * 0.5f;
            t2 += (1.5 - glm::sin(t1)) * 2 / 100;
            *(uniform_buffer_pointer0 + 1) = t2 * 1.5f;
            *(uniform_buffer_pointer1 + 0) = glm::abs((glm::sin(t1) + 1.2f) * 0.3f);
            *(uniform_buffer_pointer1 + 1) = -t1 * 2.0f;
            float t_ = t1 * 1.0;
            ctx->set_clear_color(MangoRHI::ColorClearValue { 
                .r = (glm::sin(t_) + 1.0f) / 2.0f, 
                .g = (glm::sin(t_ + 3.14159265358979f * 2.0f / 30.0f) + 1.0f) / 2.0f, 
                .b = (glm::sin(t_ + 3.14159265358979f * 4.0f / 30.0f) + 1.0f) / 2.0f, 
                .a = 1.0f } );
            auto viewport = MangoRHI::Viewport { 0, static_cast<float>(ctx->get_height()), static_cast<float>(ctx->get_width()), -static_cast<float>(ctx->get_height()), 0.0f, 1.0f };
            auto scissor = MangoRHI::Scissor { 0, 0, ctx->get_width(), ctx->get_height() };
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.bind_vertex_buffer(vertex_buffer, 0);
            command.bind_vertex_buffer(color_buffer, 1);
            command.bind_index_buffer(index_buffer);
            command.draw_indexed_instances(6, 8, 0, 0,  0);

            ctx->end_frame();
        }
    }

    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
