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

    VkPipelineStageFlags pipeline_stage2vk_pipeline_stage_flags(PipelineStage stage) {
        switch (stage) {
        case PipelineStage::eColorOutput:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
    }

    VkAccessFlags access2vk_access_flags(Access access) {
        switch (access) {
        case Access::eNone:
            return VK_ACCESS_NONE;
        case Access::eColorRenderTargetWrite:
            return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }
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
    
    VkPolygonMode polygon2vk_polygon_mode(PolygonMode polygon) {
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

    VkFormat vertex_input_type2vk_format(VertexInputType type) {
        switch (type) {
        case VertexInputType::eInt2:
            return VK_FORMAT_R32G32_SINT;
        case VertexInputType::eInt3:
            return VK_FORMAT_R32G32B32_SINT;
        case VertexInputType::eInt4:
            return VK_FORMAT_R32G32B32A32_SINT;
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
}
