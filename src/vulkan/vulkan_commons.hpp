#pragma once

#include "MangoRHI/commons.hpp"
#include <vulkan/vulkan.h>

namespace MangoRHI {
    #define VK_CHECK(expr) \
    { \
        if (expr != VK_SUCCESS) { \
            RHI_ERROR("Vulkan Error: \n{} --> {}\n\t{}", __FILE__, __LINE__, #expr) \
            RHI_ERROR("") \
        } \
    }

    using SurfaceCreateFunc = std::function<VkSurfaceKHR(VkInstance &, VkAllocationCallbacks *)>;
    struct VulkanContextInfo {
        const char **extensions = nullptr;
        u32 extension_count = 0;
        const char *app_name = "";
        const char *engine_name = "";
        SurfaceCreateFunc surface_create_callback {};
    };

    struct SwapchainDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        STL_IMPL::vector<VkSurfaceFormatKHR> formats;
        STL_IMPL::vector<VkPresentModeKHR> present_modes;
    };

    VkClearValue clear_value2vk_clear_value(ClearValue clear_value);
    VkImageLayout render_target_layout2vk_image_layout(RenderTargetLayout layout);
    VkPipelineBindPoint pipeline_bind_point2vk_pipeline_bind_point(PipelineBindPoint bind_point);
    VkCommandBufferLevel command_level2vk_command_buffer_level(CommandLevel level);
    VkPipelineStageFlags pipeline_stage2vk_pipeline_stage_flags(PipelineStage stage);
    VkAccessFlags access2vk_access_flags(Access access);
    VkPrimitiveTopology topology2vk_primitive_topology(Topology topology);
    VkPolygonMode polygon2vk_polygon_mode(PolygonMode polygon);
    VkFrontFace front_face2vk_front_face(FrontFace front);
    VkCullModeFlags cull_mode2vk_cull_mode_flags(CullMode cull);
    VkFormat vertex_input_type2vk_format(VertexInputType type);
    VkVertexInputRate vertex_input_rate2vk_vertex_input_rate(VertexInputRate rate);

    Result initialize_vulkan();
    Result quit_vulkan();
}
