#include "vulkan_commons.hpp"

namespace MangoRHI {
    Result initialize_vulkan() {
        RHI_DEBUG("initialize vulkan")
        return Result::eSuccess;
    }

    Result quit_vulkan() {
        RHI_DEBUG("quit vulkan")
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
        case RenderTargetLayout::ePresentSrc:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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
}
