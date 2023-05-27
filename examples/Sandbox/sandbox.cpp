#include "MangoRHI.hpp"
#include "GLFW/glfw3.h"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto *glfwWindow = glfwCreateWindow(640, 640, "Sandbox", nullptr, nullptr);

    MangoRHI::set_logger_level(MangoRHI::LogLevel::eDebug);
    MangoRHI::initialize(MangoRHI::API::eVulkan);
    MangoRHI::Context *ctx = MangoRHI::create_context();

    MangoRHI::VulkanContextInfo info;
    info.extensions = glfwGetRequiredInstanceExtensions(&info.extension_count);
    info.surface_create_callback = [glfwWindow](VkInstance &instance, VkAllocationCallbacks *allocator) {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        glfwCreateWindowSurface(instance, glfwWindow, allocator, &surface);
        return surface;
    };
    ctx->set_application_name("MangoRHI Sanbox App");
    ctx->set_device_name("NVIDIA GeForce RTX 4090");
    ctx->set_api_info(&info);

    ctx->set_clear_color(MangoRHI::ColorClearValue { .r = 1.0f, .g = 1.0f, .b = 0.04f, .a = 1.0f } );
    auto &rp = ctx->get_render_pass_reference();
    rp.add_output_render_target(MANGORHI_SURFACE_RENDER_TARGET_NAME, MangoRHI::RenderTargetLayout::eColor);
    rp.add_subpass("main", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_dependency({ MANGORHI_EXTERNAL_SUBPASS_NAME, MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eNone }, { "main", MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eColorRenderTargetWrite });
    
    ctx->create();

    glfwSetWindowUserPointer(glfwWindow, ctx);
    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow *window, int w, int h) {
        MangoRHI::Context *ctx = (MangoRHI::Context *)glfwGetWindowUserPointer(window);
        ctx->resize(static_cast<MangoRHI::u32>(w), static_cast<MangoRHI::u32>(h));
    });

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if (ctx->begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = ctx->get_current_command();

            // TODO: Record Command
            // vkCmdClearAttachments(((MangoRHI::VulkanCommand &)command).get_command_buffer(), 0, nullptr, 0, nullptr);

            ctx->end_frame();
        }
    }

    ctx->destroy();
    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
