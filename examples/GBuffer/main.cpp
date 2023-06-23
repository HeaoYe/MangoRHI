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
    {
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

    auto &rp = ctx.get_render_pass_reference();
    rp.create_render_target("depth", MangoRHI::RenderTargetUsage::eDepth);
    ctx.set_clear_value("depth", MangoRHI::ClearValue { .depth_stencil = { .depth = 1.0f, .stencil = 0 } });
    rp.create_render_target("color", MangoRHI::RenderTargetUsage::eColor);

    rp.create_render_target("pos", MangoRHI::RenderTargetUsage::eColor);
    rp.create_render_target("pos_resolve", MangoRHI::RenderTargetUsage::eBuffer);
    rp.create_render_target("normal", MangoRHI::RenderTargetUsage::eColor);
    rp.create_render_target("normal_resolve", MangoRHI::RenderTargetUsage::eBuffer);
    rp.create_render_target("texture", MangoRHI::RenderTargetUsage::eColor);
    rp.create_render_target("texture_resolve", MangoRHI::RenderTargetUsage::eBuffer);

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
    ctx.create();

    auto &rf = ctx.get_resource_factory_reference();
    auto sp1 = rf.create_shader_program("first");
    sp1->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    sp1->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    sp1->add_vertex_attribute(MangoRHI::VertexInputType::eFloat3, sizeof(glm::vec3));
    sp1->add_vertex_binding(MangoRHI::VertexInputRate::ePerVertex);
    auto vert = rf.create_shader("assets/shaders/vert.spv");
    auto frag = rf.create_shader("assets/shaders/frag.spv");
    sp1->attach_vertex_shader(*vert, "main");
    sp1->attach_fragment_shader(*frag, "main");
    sp1->set_cull_mode(MangoRHI::CullMode::eNone);
    sp1->set_depth_test_enabled(MangoRHI::MG_TRUE);
    sp1->set_depth_compare_op(MangoRHI::DepthCompareOp::eLessOrEqual);
    auto l1 = sp1->create_descriptor_set_layout("g-buffer");
    l1.lock()->add_textures_descriptor(MangoRHI::DescriptorStage::eFragment, 1);
    sp1->create();
    vert.reset();
    frag.reset();
    auto texture = rf.create_texture("assets/textures/dance.png");
    auto d1 = sp1->allocate_descriptor_set("g-buffer");
    d1->set_texture(0, 0, *texture);
    d1->update_all();

    auto sp2 = rf.create_shader_program("second");
    auto vert2 = rf.create_shader("assets/shaders/vert2.spv");
    auto frag2 = rf.create_shader("assets/shaders/frag2.spv");
    sp2->attach_vertex_shader(*vert2, "main");
    sp2->attach_fragment_shader(*frag2, "main");
    sp2->set_cull_mode(MangoRHI::CullMode::eNone);
    sp2->set_depth_test_enabled(MangoRHI::MG_TRUE);
    sp2->set_depth_compare_op(MangoRHI::DepthCompareOp::eLessOrEqual);
    sp2->set_depth_write_enabled(MangoRHI::MG_FALSE);
    auto l2 = sp2->create_descriptor_set_layout("render");
    l2.lock()->add_input_render_targets_descriptor(MangoRHI::DescriptorStage::eFragment, 1);
    l2.lock()->add_input_render_targets_descriptor(MangoRHI::DescriptorStage::eFragment, 1);
    l2.lock()->add_input_render_targets_descriptor(MangoRHI::DescriptorStage::eFragment, 1);
    sp2->create();
    vert2.reset();
    frag2.reset();
    auto sampler = rf.create_sampler();
    auto d2 = sp2->allocate_descriptor_set("render");
    d2->set_input_render_target(0, 0, { "pos_resolve", *sampler });
    d2->set_input_render_target(1, 0, { "normal_resolve", *sampler });
    d2->set_input_render_target(2, 0, { "texture_resolve", *sampler });
    d2->update_all();

    auto vertex_buffer = rf.create_vertex_buffer(sizeof(Vertex));
    auto index_buffer = rf.create_index_buffer();

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
    vertex_buffer->write_data(vertices.data(), 0, vertices.size());
    index_buffer->write_data(indices.data(), 0, indices.size());

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if (ctx.begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = ctx.get_current_command_reference();

            auto viewport = MangoRHI::Viewport { 0, static_cast<float>(ctx.get_height()), static_cast<float>(ctx.get_width()), -static_cast<float>(ctx.get_height()), 0.0f, 1.0f };
            auto scissor = MangoRHI::Scissor { 0, 0, ctx.get_width(), ctx.get_height() };
            command.bind_shader_program(*sp1);
            command.bind_descriptor_set(*d1);
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.bind_vertex_buffer(*vertex_buffer, 0);
            command.bind_index_buffer(*index_buffer);
            command.draw_indexed_instances(6, 1, 0, 0,  0);

            command.next_subpass();
            command.bind_shader_program(*sp2);
            command.bind_descriptor_set(*d2);
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.bind_vertex_buffer(*vertex_buffer, 0);
            command.bind_index_buffer(*index_buffer);
            command.draw_indexed_instances(3, 2, 0, 0,  0);

            ctx.end_frame();
        }
    }
    }
    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
