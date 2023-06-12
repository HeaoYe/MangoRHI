#include "MangoRHI/MangoRHI.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto *glfwWindow = glfwCreateWindow(640, 640, "Sandbox", nullptr, nullptr);

    MangoRHI::set_logger_level(MangoRHI::LogLevel::eDebug);
    // MangoRHI::set_logger_level(MangoRHI::LogLevel::eInfo);
    MangoRHI::initialize(MangoRHI::API::eVulkan);

    #define SIZE(cls) RHI_INFO("sizeof({}) = {}", #cls, sizeof(cls))
    SIZE(MangoRHI::VulkanBuffer)
    SIZE(MangoRHI::VulkanVertexBuffer)
    SIZE(MangoRHI::VulkanIndexBuffer)
    SIZE(MangoRHI::VulkanImage)
    SIZE(MangoRHI::VulkanRenderTarget)
    SIZE(MangoRHI::VulkanSampler)
    SIZE(MangoRHI::VulkanShaderProgram)
    SIZE(MangoRHI::VulkanShader)
    SIZE(MangoRHI::VulkanTexture)
    SIZE(MangoRHI::VulkanCommandPool)
    SIZE(MangoRHI::VulkanCommand)
    SIZE(MangoRHI::VulkanContext)
    SIZE(MangoRHI::VulkanDescriptorPool)
    SIZE(MangoRHI::VulkanDescriptorSet)
    SIZE(MangoRHI::VulkanUniformDescriptor)
    SIZE(MangoRHI::VulkanTextureDescriptor)
    SIZE(MangoRHI::VulkanInputRenderTargetDescriptor)
    SIZE(MangoRHI::VulkanDevice)
    SIZE(MangoRHI::VulkanFrameBuffer)
    SIZE(MangoRHI::VulkanRenderPass)
    SIZE(MangoRHI::VulkanSubpass)
    SIZE(MangoRHI::VulkanResourceFactory)
    SIZE(MangoRHI::VulkanSwapchain)
    SIZE(MangoRHI::VulkanSynchronization)

    MangoRHI::Context &ctx = MangoRHI::get_context();
    #ifdef MANGORHI_DEBUG
    RHI_WARN("Debug Mode")
    #endif
    {
    MangoRHI::VulkanContextInfo info;
    info.extensions = glfwGetRequiredInstanceExtensions(&info.extension_count);
    info.surface_create_callback = [glfwWindow](VkInstance instance, VkAllocationCallbacks *allocator) {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        glfwCreateWindowSurface(instance, glfwWindow, allocator, &surface);
        return surface;
    };
    info.app_name = "MangoRHI Sanbox App";
    info.engine_name = "No Engine";
    ctx.set_api_info(&info);
    ctx.set_device_name("NVIDIA GeForce RTX 4090");
    ctx.set_vsync_enabled(MangoRHI::MG_TRUE);
    ctx.set_swapchain_image_count(3);
    ctx.set_max_in_flight_frame_count(2);
    ctx.set_multisample_count(MangoRHI::MultisampleCount::e8);

    auto &rp = ctx.get_render_pass_reference();
    rp.create_render_target("color", MangoRHI::RenderTargetUsage::eColor);
    rp.create_render_target("depth", MangoRHI::RenderTargetUsage::eDepth);
    ctx.set_clear_value("depth", MangoRHI::ClearValue { .depth_stencil = { .depth = 1.0f, .stencil = 0 } });
    rp.add_output_render_target("color", MangoRHI::RenderTargetLayout::eColor, {
        .src_color_factor = MangoRHI::BlendFactor::eSrcAlpha,
        .dst_color_factor = MangoRHI::BlendFactor::eOneMinusSrcAlpha,
        .color_op = MangoRHI::BlendOp::eAdd,
        .src_alpha_factor = MangoRHI::BlendFactor::eOne,
        .dst_alpha_factor = MangoRHI::BlendFactor::eZero,
        .alpha_op = MangoRHI::BlendOp::eAdd,
    });
    rp.set_depth_render_target("depth", MangoRHI::RenderTargetLayout::eDepth);
    rp.add_resolve_render_target(MANGORHI_SURFACE_RENDER_TARGET_NAME, MangoRHI::RenderTargetLayout::eColor);
    rp.add_subpass("main", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_dependency({ MANGORHI_EXTERNAL_SUBPASS_NAME, MangoRHI::PipelineStage::eColorOutput | MangoRHI::PipelineStage::eEarlyFragmentTest, MangoRHI::Access::eNone }, { "main", MangoRHI::PipelineStage::eColorOutput | MangoRHI::PipelineStage::eEarlyFragmentTest, MangoRHI::Access::eColorRenderTargetWrite | MangoRHI::Access::eDepthStencilRenderTargetWrite });

    ctx.create();

    auto &rf = ctx.get_resource_factory_reference();

    auto t_61 = rf.create_texture("assets/textures/61.png");
    auto t_paper_plane= rf.create_texture("assets/textures/paper plane.png");
    auto t_dance = rf.create_texture("assets/textures/dance.png");
    auto t_dhl = rf.create_texture("assets/textures/dhl.png");
    auto t_tm = rf.create_texture("assets/textures/tm.png");

    auto main_shader_program = rf.create_shader_program("main");
    main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    main_shader_program->add_vertex_binding(MangoRHI::VertexInputRate::ePerVertex);
    main_shader_program->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    main_shader_program->add_vertex_binding(MangoRHI::VertexInputRate::ePerInstance);
    auto vert_shader = rf.create_shader("assets/shaders/vert.spv");
    auto frag_shader = rf.create_shader("assets/shaders/frag.spv");
    main_shader_program->attach_vertex_shader(*vert_shader, "main");
    main_shader_program->attach_fragment_shader(*frag_shader, "main");
    vert_shader.release()->destroy_before(main_shader_program.get());
    auto *frag_shader_p = frag_shader.release();
    frag_shader_p->destroy_before(main_shader_program.get());
    main_shader_program->remove_destroy_dependency(frag_shader_p);
    main_shader_program->set_cull_mode(MangoRHI::CullMode::eNone);
    main_shader_program->set_depth_test_enabled(MangoRHI::MG_TRUE);
    main_shader_program->set_depth_compare_op(MangoRHI::DepthCompareOp::eLessOrEqual);

    auto ds = main_shader_program->create_descriptor_set();
    MangoRHI::u32 uniform_binding = ds.lock()->add_uniforms_descriptor(MangoRHI::DescriptorStage::eVertex, sizeof(float) * 2, 2);
    MangoRHI::u32 textures_binding = ds.lock()->add_textures_descriptor(MangoRHI::DescriptorStage::eFragment, { *t_61.get(), *t_paper_plane.get(), *t_dance.get(), *t_dhl.get() });
    main_shader_program->create();
    frag_shader_p->destroy();

    auto vertex_buffer = rf.create_vertex_buffer(sizeof(glm::vec3));
    auto color_buffer = rf.create_vertex_buffer(sizeof(glm::vec3));
    auto index_buffer = rf.create_index_buffer();

    struct UserPointer {
        MangoRHI::DescriptorSet &ds;
        std::vector<MangoRHI::Texture *>textures;
        MangoRHI::u32 textures_binding;
    } up { *ds.lock(), { t_61.get(), t_paper_plane.get(), t_dance.get(), t_dhl.get(), t_tm.get() }, textures_binding };
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
        case 54:
            idx = 5;
            break;
        default:
            return;
        }
        for (int i = 0; i < 4; i++) {
            up->ds.set_texture(up->textures_binding, i, *up->textures[idx == 5 ? i : idx]);
        }
        up->ds.update_all();
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

        if (ctx.begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = ctx.get_current_command_reference();

            float *uniform_buffer_pointer0 = (float *)ds.lock()->get_uniform_buffer_pointer(uniform_binding, 0);
            float *uniform_buffer_pointer1 = (float *)ds.lock()->get_uniform_buffer_pointer(uniform_binding, 1);
            static float t1 = 0.0f, t2 = 0.0f;
            t1 += 0.01f;
            *(uniform_buffer_pointer0 + 0) = (glm::sin(t1) + 0.2f) * 0.5f;
            t2 += (1.5 - glm::sin(t1)) * 2 / 100;
            *(uniform_buffer_pointer0 + 1) = t2 * 1.5f;
            *(uniform_buffer_pointer1 + 0) = glm::abs((glm::sin(t1) + 1.2f) * 0.3f);
            *(uniform_buffer_pointer1 + 1) = -t1 * 2.0f;
            float t_ = t1 * 1.0;
            ctx.set_clear_value("color", {
                .color = {
                    .r = (glm::sin(t_) + 1.0f) / 2.0f,
                    .g = (glm::sin(t_ + 3.14159265358979f * 2.0f / 30.0f) + 1.0f) / 2.0f,
                    .b = (glm::sin(t_ + 3.14159265358979f * 4.0f / 30.0f) + 1.0f) / 2.0f,
                    .a = 1.0f
            }});
            auto viewport = MangoRHI::Viewport { 0, static_cast<float>(ctx.get_height()), static_cast<float>(ctx.get_width()), -static_cast<float>(ctx.get_height()), 0.0f, 1.0f };
            auto scissor = MangoRHI::Scissor { 0, 0, ctx.get_width(), ctx.get_height() };
            command.bind_shader_program(*main_shader_program);
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.bind_vertex_buffer(*vertex_buffer, 0);
            command.bind_vertex_buffer(*color_buffer, 1);
            command.bind_index_buffer(*index_buffer);
            command.draw_indexed_instances(6, 8, 0, 0,  0);

            ctx.end_frame();
        }
    }
    }

    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
