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
    ctx->resize(640, 640);
    ctx->set_application_name("MangoRHI Sanbox App");
    // ctx->set_device_name("NVIDIA GeForce RTX 4090");
    ctx->set_api_info(&info);

    ctx->create();

    // TODO

    ctx->destroy();
    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
