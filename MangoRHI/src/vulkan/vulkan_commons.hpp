#pragma once

#include "MangoRHI/commons.hpp"
#include <vulkan/vulkan.h>

namespace MangoRHI {
    static VkResult __res;
    #define VK_CHECK(expr) \
    { \
        if ((__res = expr) != VK_SUCCESS) { \
            RHI_ERROR("Vulkan Error {}: \n{} --> {}\n\t{}", (i32)__res, __FILE__, __LINE__, #expr) \
        } \
    }

    using SurfaceCreateFunc = std::function<VkSurfaceKHR(VkInstance, VkAllocationCallbacks *)>;
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

    VkBool32 bool2vk_bool32(Bool b);
    VkClearValue clear_value2vk_clear_value(ClearValue clear_value);
    VkImageLayout render_target_layout2vk_image_layout(RenderTargetLayout layout);
    VkPipelineBindPoint pipeline_bind_point2vk_pipeline_bind_point(PipelineBindPoint bind_point);
    VkCommandBufferLevel command_level2vk_command_buffer_level(CommandLevel level);
    VkPipelineStageFlags pipeline_stage2vk_pipeline_stage_flags(PipelineStageFlags stage);
    VkPipelineStageFlags pipeline_stage2vk_pipeline_stage_flags(PipelineStage stage);
    VkAccessFlags access2vk_access_flags(AccessFlags access);
    VkAccessFlags access2vk_access_flags(Access access);
    VkPrimitiveTopology topology2vk_primitive_topology(Topology topology);
    VkPolygonMode polygon_mode2vk_polygon_mode(PolygonMode polygon);
    VkFrontFace front_face2vk_front_face(FrontFace front);
    VkCullModeFlags cull_mode2vk_cull_mode_flags(CullMode cull);
    VkCompareOp depth_compare_op2vk_compare_op(DepthCompareOp op);
    VkFormat vertex_input_type2vk_format(VertexInputType type);
    VkVertexInputRate vertex_input_rate2vk_vertex_input_rate(VertexInputRate rate);
    VkShaderStageFlags descriptor_stage2vk_shader_stage_flags(DescriptorStageFlags stage);
    VkShaderStageFlags descriptor_stage2vk_shader_stage_flags(DescriptorStage stage);
    VkFilter sampler_filter2vk_filter(SamplerFilter filter);
    VkSamplerAddressMode sampler_address_mode2vk_sampler_address_mode(SamplerAddressMode mode);
    VkBorderColor sampler_border_color2vk_border_color(SamplerBorderColor color);
    VkSamplerMipmapMode sampler_filter2vk_sampler_mipmap_mode(SamplerFilter filter);
    VkSampleCountFlagBits multisample_count2vk_sample_count(MultisampleCount count);
    Bool check_color_blend_info_enable(ColorBlendInfo &info);
    VkBlendFactor blend_factor2vk_blend_factor(BlendFactor factor);
    VkBlendOp blend_op2vk_blend_op(BlendOp op);
    VkDynamicState dynamic_state2vk_dynamic_state(DynamicState state);

    Result initialize_vulkan();
    Result quit_vulkan();
}
