#include "MangoRHI/MangoRHI.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto *glfwWindow = glfwCreateWindow(640, 640, "Sandbox", nullptr, nullptr);

    MangoRHI::set_logger_level(MangoRHI::LogLevel::eDebug);
    MangoRHI::initialize(MangoRHI::API::eVulkan);
    MangoRHI::Context &ctx = MangoRHI::get_context();

    MangoRHI::VulkanContextInfo info;
    info.extensions = glfwGetRequiredInstanceExtensions(&info.extension_count);
    info.surface_create_callback = [glfwWindow](VkInstance instance, VkAllocationCallbacks *allocator) {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        glfwCreateWindowSurface(instance, glfwWindow, allocator, &surface);
        return surface;
    };
    info.app_name = "MangoRHI GBuffer";
    info.engine_name = "No Engine";
    ctx.set_api_info(&info);
    ctx.set_device_name("NVIDIA GeForce RTX 4090");
    ctx.set_vsync_enabled(MangoRHI::MG_FALSE);
    ctx.set_swapchain_image_count(3);
    ctx.set_max_in_flight_frame_count(2);
    ctx.set_multisample_count(MangoRHI::MultisampleCount::e8);
    auto &rm = ctx.get_resource_manager_reference();

    rm.create_render_target("depth", MangoRHI::RenderTargetUsage::eDepth).set_clear_color(MangoRHI::ClearValue { .depth_stencil = { .depth = 1.0f, .stencil = 0 } });
    rm.create_render_target("color", MangoRHI::RenderTargetUsage::eColor);

    rm.create_render_target("pos", MangoRHI::RenderTargetUsage::eColor);
    rm.create_render_target("pos_resolve", MangoRHI::RenderTargetUsage::eColorBuffer);
    rm.create_render_target("normal", MangoRHI::RenderTargetUsage::eColor);
    rm.create_render_target("normal_resolve", MangoRHI::RenderTargetUsage::eColorBuffer);
    rm.create_render_target("texture", MangoRHI::RenderTargetUsage::eColor);
    rm.create_render_target("texture_resolve", MangoRHI::RenderTargetUsage::eColorBuffer);

    auto &rp = ctx.get_render_pass_reference();
    rp.add_output_render_target("pos", MangoRHI::RenderTargetLayout::eColor);
    rp.add_resolve_render_target("pos_resolve", MangoRHI::RenderTargetLayout::eColor);
    rp.add_output_render_target("normal", MangoRHI::RenderTargetLayout::eColor);
    rp.add_resolve_render_target("normal_resolve", MangoRHI::RenderTargetLayout::eColor);
    rp.add_output_render_target("texture", MangoRHI::RenderTargetLayout::eColor);
    rp.add_resolve_render_target("texture_resolve", MangoRHI::RenderTargetLayout::eColor);
    rp.set_depth_render_target("depth", MangoRHI::RenderTargetLayout::eDepth);
    rp.add_subpass("first", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_input_render_target("pos_resolve", MangoRHI::RenderTargetLayout::eShaderRead);
    rp.add_input_render_target("normal_resolve", MangoRHI::RenderTargetLayout::eShaderRead);
    rp.add_input_render_target("texture_resolve", MangoRHI::RenderTargetLayout::eShaderRead);
    rp.add_output_render_target("color", MangoRHI::RenderTargetLayout::eColor);
    rp.set_depth_render_target("depth", MangoRHI::RenderTargetLayout::eDepthReadonly);
    rp.add_resolve_render_target(MANGORHI_SURFACE_RENDER_TARGET_NAME, MangoRHI::RenderTargetLayout::eColor);
    rp.add_subpass("second", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_dependency({
        MANGORHI_EXTERNAL_SUBPASS_NAME,
        MangoRHI::PipelineStage::eEarlyFragmentTest | MangoRHI::PipelineStage::eLateFragmentTest,
        MangoRHI::Access::eDepthStencilRenderTargetRead,
    }, {
        "first",
        MangoRHI::PipelineStage::eEarlyFragmentTest | MangoRHI::PipelineStage::eLateFragmentTest,
        MangoRHI::Access::eDepthStencilRenderTargetRead | MangoRHI::Access::eDepthStencilRenderTargetWrite,
    });
    rp.add_dependency({
        MANGORHI_EXTERNAL_SUBPASS_NAME,
        MangoRHI::PipelineStage::eBottomOfPipeline,
        MangoRHI::Access::eMemoryRead,
    }, {
        "first",
        MangoRHI::PipelineStage::eColorOutput,
        MangoRHI::Access::eColorRenderTargetRead | MangoRHI::Access::eColorRenderTargetWrite,
    });
    rp.add_dependency({
        "first",
        MangoRHI::PipelineStage::eColorOutput,
        MangoRHI::Access::eColorRenderTargetWrite,
    }, {
        "second",
        MangoRHI::PipelineStage::eFragmentShader,
        MangoRHI::Access::eShaderRead,
    });
    rp.add_dependency({
        "second",
        MangoRHI::PipelineStage::eColorOutput,
        MangoRHI::Access::eColorRenderTargetRead | MangoRHI::Access::eColorRenderTargetWrite,
    }, {
        MANGORHI_EXTERNAL_SUBPASS_NAME,
        MangoRHI::PipelineStage::eBottomOfPipeline,
        MangoRHI::Access::eMemoryRead,
    });

    auto &sp1 = rm.create_shader_program("first");
    auto &sp2 = rm.create_shader_program("second");
    sp1.add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    sp1.add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    sp1.add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    sp1.add_vertex_binding(MangoRHI::VertexInputRate::ePerVertex);
    sp1.attach_vertex_shader(rm.create_shader("assets/shaders/vert.spv"), "main");
    sp1.attach_fragment_shader(rm.create_shader("assets/shaders/frag.spv"), "main");
    sp1.set_cull_mode(MangoRHI::CullMode::eNone);
    sp1.set_depth_test_enabled(MangoRHI::MG_TRUE);
    sp1.set_depth_compare_op(MangoRHI::DepthCompareOp::eLessOrEqual);
    auto &texture = rm.create_texture("assets/textures/dance.png");
    auto &ds1 = sp1.create_descriptor_set();
    ds1.add_textures(MangoRHI::DescriptorStage::eFragment, { texture });

    sp2.attach_vertex_shader(rm.create_shader("assets/shaders/vert2.spv"), "main");
    sp2.attach_fragment_shader(rm.create_shader("assets/shaders/frag2.spv"), "main");
    sp2.set_cull_mode(MangoRHI::CullMode::eNone);
    sp2.set_depth_test_enabled(MangoRHI::MG_TRUE);
    sp2.set_depth_compare_op(MangoRHI::DepthCompareOp::eLessOrEqual);
    sp2.set_depth_write_enabled(MangoRHI::MG_FALSE);
    auto &ds2 = sp2.create_descriptor_set();
    auto &sampler = rm.create_sampler();
    ds2.add_input_render_targets(MangoRHI::DescriptorStage::eFragment, { { "pos_resolve", sampler } });
    ds2.add_input_render_targets(MangoRHI::DescriptorStage::eFragment, { { "normal_resolve", sampler } });
    ds2.add_input_render_targets(MangoRHI::DescriptorStage::eFragment, { { "texture_resolve", sampler } });

    auto &vertex_buffer = rm.create_vertex_buffer(sizeof(Vertex));
    auto &index_buffer = rm.create_index_buffer();

    ctx.create();

    const std::vector<Vertex> vertices = {
        {
            { 0.5f, 0.5f, 0.1f },
            { 0.0f, 0.0f, 0.1f },
            { 0.5f, 0.5f, 0.5f },
        },
        {
            { 0.5f, -0.5f, 0.2f },
            { 0.0f, 0.1f, 0.1f },
            { 0.5f, 0.5f, 0.5f },
        },
        {
            { -0.5f, -0.5f, 0.3f },
            { 0.0f, 0.2f, 0.1f },
            { 0.5f, 0.5f, 0.5f },
        },
        {
            { -0.5f, 0.5f, 0.4f },
            { 0.1f, 0.0f, 0.1f },
            { 0.5f, 0.5f, 0.5f },
        },
    };
    std::vector<MangoRHI::u32> indices = {
        0, 1, 2,
        0, 3, 2,
    };
    vertex_buffer.write_data(vertices.data(), vertices.size(), 0);
    index_buffer.write_data(indices.data(), indices.size(), 0);

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if (ctx.begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = ctx.get_current_command_reference();

            auto viewport = MangoRHI::Viewport { 0, static_cast<float>(ctx.get_height()), static_cast<float>(ctx.get_width()), -static_cast<float>(ctx.get_height()), 0.0f, 1.0f };
            auto scissor = MangoRHI::Scissor { 0, 0, ctx.get_width(), ctx.get_height() };
            command.bind_shader_program(sp1);
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.bind_vertex_buffer(vertex_buffer, 0);
            command.bind_index_buffer(index_buffer);
            command.draw_indexed_instances(6, 1, 0, 0,  0);

            command.next_subpass();
            command.bind_shader_program(sp2);
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.bind_vertex_buffer(vertex_buffer, 0);
            command.bind_index_buffer(index_buffer);
            command.draw_indexed_instances(3, 2, 0, 0,  0);

            ctx.end_frame();
        }
    }

    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
