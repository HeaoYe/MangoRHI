#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "MangoRHI/MangoRHI.hpp"
#include <GLFW/glfw3.h>

void vk_check_fn(VkResult res) {
    if (res != VK_SUCCESS) {
        RHI_ERROR("Vk Check Failed - {}", (int)res)
    }
}

extern void ImGui_ImplVulkanH_CreateWindowCommandBuffers(VkPhysicalDevice physical_device, VkDevice device, ImGui_ImplVulkanH_Window* wd, uint32_t queue_family, const VkAllocationCallbacks* allocator);
extern void ImGui_ImplVulkanH_DestroyFrameSemaphores(VkDevice device, ImGui_ImplVulkanH_FrameSemaphores* fsd, const VkAllocationCallbacks* allocator);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* glfwWindow = glfwCreateWindow(1280, 720, "ImGui with MangoRHI", nullptr, nullptr);

    MangoRHI::set_logger_level(MangoRHI::LogLevel::eDebug);
    MangoRHI::initialize(MangoRHI::API::eVulkan);
    MangoRHI::Context &ctx = MangoRHI::get_context();
    auto &vk_ctx = (MangoRHI::VulkanContext &)ctx;

    MangoRHI::VulkanContextInfo info;
    info.extensions = glfwGetRequiredInstanceExtensions(&info.extension_count);
    info.surface_create_callback = [glfwWindow](VkInstance instance, VkAllocationCallbacks *allocator) {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        glfwCreateWindowSurface(instance, glfwWindow, allocator, &surface);
        return surface;
    };
    info.app_name = "ImGui With MangoRHI";
    info.engine_name = "No Engine";
    ctx.set_api_info(&info);
    ctx.set_device_name("NVIDIA GeForce RTX 4090");
    ctx.set_vsync_enabled(MangoRHI::MG_FALSE);
    ctx.set_swapchain_image_count(3);
    ctx.set_max_in_flight_frame_count(2);
    ctx.set_multisample_count(MangoRHI::MultisampleCount::e8);
    auto &rm = ctx.get_resource_manager_reference();

    rm.create_render_target("resolve", MangoRHI::RenderTargetUsage::eColor);
    auto &rp = ctx.get_render_pass_reference();
    rp.add_output_render_target("resolve", MangoRHI::RenderTargetLayout::eColor);
    rp.add_subpass("main", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_output_render_target("resolve", MangoRHI::RenderTargetLayout::eColor);
    rp.add_resolve_render_target(MANGORHI_SURFACE_RENDER_TARGET_NAME, MangoRHI::RenderTargetLayout::eColor);
    rp.add_subpass("imgui", MangoRHI::PipelineBindPoint::eGraphicsPipeline);
    rp.add_dependency({ MANGORHI_EXTERNAL_SUBPASS_NAME, MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eNone }, { "main", MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eColorRenderTargetWrite });
    rp.add_dependency({ "main", MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eColorRenderTargetWrite }, { "imgui", MangoRHI::PipelineStage::eColorOutput, MangoRHI::Access::eColorRenderTargetWrite });
    auto &sp = rm.create_shader_program("main");
    sp.set_cull_mode(MangoRHI::CullMode::eNone);
    sp.attach_vertex_shader(rm.create_shader("assets/shaders/vert.spv"), "main");
    sp.attach_fragment_shader(rm.create_shader("assets/shaders/frag.spv"), "main");

    ctx.create();

    VkDescriptorPool g_DescriptorPool;
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    vkCreateDescriptorPool(vk_ctx.get_device()->get_logical_device(), &pool_info, vk_ctx.get_allocator(), &g_DescriptorPool);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
    ImGui_ImplVulkan_InitInfo init_info {};
    init_info.Instance = vk_ctx.get_instance();
    init_info.PhysicalDevice = vk_ctx.get_device()->get_physical_device();
    init_info.Device = vk_ctx.get_device()->get_logical_device();
    init_info.QueueFamily = vk_ctx.get_device()->get_graphics_family_index();
    init_info.Queue = vk_ctx.get_device()->get_graphics_queue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = g_DescriptorPool;
    init_info.Subpass = vk_ctx.get_render_pass()->get_subpass_index_by_name("imgui");
    init_info.MinImageCount = vk_ctx.get_swapchain()->get_image_count();
    init_info.ImageCount = vk_ctx.get_swapchain()->get_image_count();
    init_info.MSAASamples = vk_ctx.get_multisample_count();
    init_info.Allocator = vk_ctx.get_allocator();
    init_info.CheckVkResultFn = vk_check_fn;
    ImGui_ImplVulkan_Init(&init_info, vk_ctx.get_render_pass()->get_render_pass());

    {
        MangoRHI::VulkanCommand command;
        vk_ctx.get_command_pool()->allocate_single_use(command);
        ImGui_ImplVulkan_CreateFontsTexture(command.get_command_buffer());
        vk_ctx.get_command_pool()->free(command);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();

        if (ctx.begin_frame() == MangoRHI::Result::eSuccess) {
            auto &command = ctx.get_current_command_reference();
            auto &vk_cmd = (MangoRHI::VulkanCommand &)command;

            auto viewport = MangoRHI::Viewport { 0, static_cast<float>(ctx.get_height()), static_cast<float>(ctx.get_width()), -static_cast<float>(ctx.get_height()), 0.0f, 1.0f };
            auto scissor = MangoRHI::Scissor { 0, 0, ctx.get_width(), ctx.get_height() };
            command.bind_shader_program(sp);
            command.set_viewport(viewport);
            command.set_scissor(scissor);
            command.draw_instances(3, 1, 0, 0);

            command.next_subpass();
            ImGui_ImplVulkan_RenderDrawData(draw_data, vk_cmd.get_command_buffer());
            ctx.end_frame();
        }
    }

    vkDeviceWaitIdle(vk_ctx.get_device()->get_logical_device());

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(vk_ctx.get_device()->get_logical_device(), g_DescriptorPool, vk_ctx.get_allocator());
    MangoRHI::quit();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return 0;
}
