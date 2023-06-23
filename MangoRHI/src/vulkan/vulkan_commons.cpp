#include "vulkan_commons.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    Result initialize_vulkan() {
        RHI_INFO("Initialize vulkan")
        vulkan_context = new VulkanContext();
        return Result::eSuccess;
    }

    Result quit_vulkan() {
        vulkan_context->destroy();
        RHI_INFO("Quit vulkan")
        return Result::eSuccess;
    }

    #define CASE_OR(enum_class, condition, input, flag_bit) \
    if ((((u32)input.bits) & ((u32)enum_class::condition)) != 0) { \
        flags |= flag_bit; \
    }

    VkBool32 bool2vk_bool32(Bool b) {
        if (b == MG_TRUE) {
            return VK_TRUE;
        }
        if (b == MG_FALSE) {
            return VK_FALSE;
        }
        return VK_FALSE;
    }

    VkClearValue clear_value2vk_clear_value(ClearValue clear_value) {
        VkClearValue vk_clear_value {
            .color = { .float32 = { clear_value.color.r, clear_value.color.g, clear_value.color.b, clear_value.color.a } }
        };
        return vk_clear_value;
    }

    VkImageLayout render_target_layout2vk_image_layout(RenderTargetLayout layout) {
        switch (layout) {
        case RenderTargetLayout::eUndefined:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case RenderTargetLayout::eColor:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case RenderTargetLayout::eDepth:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case RenderTargetLayout::eDepthReadonly:
            return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
        case RenderTargetLayout::eShaderRead:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
    }

    VkPipelineBindPoint pipeline_bind_point2vk_pipeline_bind_point(PipelineBindPoint bind_point) {
        switch (bind_point) {
        case PipelineBindPoint::eGraphicsPipeline:
            return VK_PIPELINE_BIND_POINT_GRAPHICS;
        case PipelineBindPoint::eComputePipeline:
            return VK_PIPELINE_BIND_POINT_COMPUTE;
        }
    }

    VkCommandBufferLevel command_level2vk_command_buffer_level(CommandLevel level) {
        switch (level) {
        case CommandLevel::ePrimary:
            return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        case CommandLevel::eSecondary:
            return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        }
    }

    VkPipelineStageFlags pipeline_stage2vk_pipeline_stage_flags(PipelineStageFlags stage) {
        VkPipelineStageFlags flags = 0;
        CASE_OR(PipelineStage, eColorOutput, stage, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
        CASE_OR(PipelineStage, eEarlyFragmentTest, stage, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
        CASE_OR(PipelineStage, eLateFragmentTest, stage, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT)
        CASE_OR(PipelineStage, eFragmentShader, stage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
        CASE_OR(PipelineStage, eBottomOfPipeline, stage, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
        CASE_OR(PipelineStage, eTopOfPipeline, stage, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
        return flags;
    }

    VkPipelineStageFlags pipeline_stage2vk_pipeline_stage_flags(PipelineStage stage) {
        return pipeline_stage2vk_pipeline_stage_flags((PipelineStageFlags)(u32)stage);
    }

    VkAccessFlags access2vk_access_flags(AccessFlags access) {
        VkAccessFlags flags = 0;
        CASE_OR(Access, eColorRenderTargetRead, access, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT)
        CASE_OR(Access, eColorRenderTargetWrite, access, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
        CASE_OR(Access, eDepthStencilRenderTargetRead, access, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT)
        CASE_OR(Access, eDepthStencilRenderTargetWrite, access, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
        CASE_OR(Access, eMemoryRead, access, VK_ACCESS_MEMORY_READ_BIT)
        CASE_OR(Access, eShaderRead, access, VK_ACCESS_SHADER_READ_BIT)
        return flags;
    }

    VkAccessFlags access2vk_access_flags(Access access) {
        return access2vk_access_flags((AccessFlags)(u32)access);
    }

    VkPrimitiveTopology topology2vk_primitive_topology(Topology topology) {
        switch (topology) {
        case Topology::ePointList:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case Topology::eLineList:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case Topology::eTriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
    }

    VkPolygonMode polygon_mode2vk_polygon_mode(PolygonMode polygon) {
        switch (polygon) {
        case PolygonMode::ePoint:
            return VK_POLYGON_MODE_POINT;
        case PolygonMode::eLine:
            return VK_POLYGON_MODE_LINE;
        case PolygonMode::eFill:
            return VK_POLYGON_MODE_FILL;
        }
    }

    VkFrontFace front_face2vk_front_face(FrontFace front) {
        switch (front) {
        case FrontFace::eCouterClockwise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case FrontFace::eClockwise:
            return VK_FRONT_FACE_CLOCKWISE;
        }
    }

    VkCullModeFlags cull_mode2vk_cull_mode_flags(CullMode cull) {
        switch (cull) {
        case CullMode::eNone:
            return VK_CULL_MODE_NONE;
        case CullMode::eBack:
            return VK_CULL_MODE_BACK_BIT;
        case CullMode::eFront:
            return VK_CULL_MODE_FRONT_BIT;
        case CullMode::eAll:
            return VK_CULL_MODE_FRONT_AND_BACK;
        }
    }

    VkCompareOp depth_compare_op2vk_compare_op(DepthCompareOp op) {
        switch (op) {
        case DepthCompareOp::eLess:
            return VK_COMPARE_OP_LESS;
        case DepthCompareOp::eLessOrEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        }
    }

    VkFormat vertex_input_type2vk_format(VertexInputType type) {
        switch (type) {
        case VertexInputType::eInt:
            return VK_FORMAT_R32_SINT;
        case VertexInputType::eInt2:
            return VK_FORMAT_R32G32_SINT;
        case VertexInputType::eInt3:
            return VK_FORMAT_R32G32B32_SINT;
        case VertexInputType::eInt4:
            return VK_FORMAT_R32G32B32A32_SINT;
        case VertexInputType::eFloat:
            return VK_FORMAT_R32_SFLOAT;
        case VertexInputType::eFloat2:
            return VK_FORMAT_R32G32_SFLOAT;
        case VertexInputType::eFloat3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case VertexInputType::eFloat4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
    }

    VkVertexInputRate vertex_input_rate2vk_vertex_input_rate(VertexInputRate rate) {
        switch (rate) {
        case VertexInputRate::ePerVertex:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        case VertexInputRate::ePerInstance:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
        }
    }

    VkShaderStageFlags descriptor_stage2vk_shader_stage_flags(DescriptorStageFlags stage) {
        VkShaderStageFlags flags = 0;
        CASE_OR(DescriptorStage, eVertex, stage, VK_SHADER_STAGE_VERTEX_BIT)
        CASE_OR(DescriptorStage, eFragment, stage, VK_SHADER_STAGE_FRAGMENT_BIT)
        return flags;
    }

    VkShaderStageFlags descriptor_stage2vk_shader_stage_flags(DescriptorStage stage) {
        return descriptor_stage2vk_shader_stage_flags((DescriptorStageFlags)(u32)stage);
    }

    VkFilter sampler_filter2vk_filter(SamplerFilter filter) {
        switch (filter) {
        case SamplerFilter::eNearest:
            return VK_FILTER_NEAREST;
        case SamplerFilter::eLinear:
            return VK_FILTER_LINEAR;
        }
    }

    VkSamplerAddressMode sampler_address_mode2vk_sampler_address_mode(SamplerAddressMode mode) {
        switch (mode) {
        case SamplerAddressMode::eRepeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerAddressMode::eMirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerAddressMode::eClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerAddressMode::eClampToBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        }
    }

    VkBorderColor sampler_border_color2vk_border_color(SamplerBorderColor color) {
        switch (color) {
        case SamplerBorderColor::eFloatTransparentBlack:
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case SamplerBorderColor::eIntTransparentBlack:
            return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        case SamplerBorderColor::eFloatOpaqueBlack:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case SamplerBorderColor::eIntOpaqueBlack:
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        case SamplerBorderColor::eFloatOpaqueWhite:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        case SamplerBorderColor::eIntOpaqueWhite:
            return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        }
    }

    VkSamplerMipmapMode sampler_filter2vk_sampler_mipmap_mode(SamplerFilter filter) {
        switch (filter) {
        case SamplerFilter::eNearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case SamplerFilter::eLinear:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }

    VkSampleCountFlagBits multisample_count2vk_sample_count(MultisampleCount count) {
        switch (count) {
        case MultisampleCount::e1:
            return VK_SAMPLE_COUNT_1_BIT;
        case MultisampleCount::e2:
            return VK_SAMPLE_COUNT_2_BIT;
        case MultisampleCount::e4:
            return VK_SAMPLE_COUNT_4_BIT;
        case MultisampleCount::e8:
            return VK_SAMPLE_COUNT_8_BIT;
        case MultisampleCount::e16:
            return VK_SAMPLE_COUNT_16_BIT;
        case MultisampleCount::e32:
            return VK_SAMPLE_COUNT_32_BIT;
        case MultisampleCount::e64:
            return VK_SAMPLE_COUNT_64_BIT;
        }
    }

    Bool check_color_blend_info_enable(ColorBlendInfo &info) {
        return (
            (info.src_color_factor != BlendFactor::eNone) && (info.dst_color_factor != BlendFactor::eNone) && (info.color_op != BlendOp::eNone) &&
            (info.src_alpha_factor != BlendFactor::eNone) && (info.dst_alpha_factor != BlendFactor::eNone) && (info.alpha_op != BlendOp::eNone)
        ) ? MG_TRUE : MG_FALSE;
    }

    VkBlendFactor blend_factor2vk_blend_factor(BlendFactor factor) {
        switch (factor) {
        case BlendFactor::eNone:
            return (VkBlendFactor)0;
        case BlendFactor::eZero:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::eOne:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::eSrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::eOneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::eDstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::eOneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        }
    }

    VkBlendOp blend_op2vk_blend_op(BlendOp op) {
        switch (op) {
        case BlendOp::eNone:
            return (VkBlendOp)0;
        case BlendOp::eAdd:
            return VK_BLEND_OP_ADD;
        case BlendOp::eSub:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOp::eReverseSub:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::eMin:
            return VK_BLEND_OP_MIN;
        case BlendOp::eMax:
            return VK_BLEND_OP_MAX;
        }
    }

    VkDynamicState dynamic_state2vk_dynamic_state(DynamicState state) {
        switch (state) {
        case DynamicState::eDepthTestEnable:
            return VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE;
        case DynamicState::eDepthWriteEnable:
            return VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE;
        }
    }

    #undef CASE_OR
}
