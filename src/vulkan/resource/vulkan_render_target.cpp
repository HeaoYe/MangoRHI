#include "vulkan_render_target.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanRenderTarget::set_usage(RenderTargetUsage usage) {
        switch (usage) {
        case RenderTargetUsage::eColor:
            description.format = vulkan_context->get_swapchain().get_format().format;
            description.samples = VK_SAMPLE_COUNT_1_BIT;
            description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            this->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            this->aspect = VK_IMAGE_ASPECT_COLOR_BIT;
            break;
        case RenderTargetUsage::eDepth:
            description.format = VK_FORMAT_D32_SFLOAT;
            description.samples = VK_SAMPLE_COUNT_1_BIT;
            description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            this->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            this->aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
            break;
        }
    }

    void VulkanRenderTarget::add_render_target_data(VkImage image, VkImageView image_view) {
        this->images.push_back(image);
        this->image_views.push_back(image_view);
        if (images.size() > 1) {
            is_each_frame_render_target = MG_TRUE;
        }
    }

    Result VulkanRenderTarget::create() {
        component_create()

        if (images.size() == 0) {
            if (is_each_frame_render_target == MG_TRUE) {
                vulkan_images.resize(vulkan_context->get_swapchain().get_image_count());
            } else {
                vulkan_images.resize(1);
            }
        }
        for (auto &image : vulkan_images) {
            image = new VulkanImage();
            image->set_extent(vulkan_context->get_extent());
            image->set_format(description.format);
            image->set_usage(usage);
            image->set_aspect(aspect);
            image->create();
            vulkan_context->transition_image_layout(image->get_image(), image->get_format(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
            images.push_back(image->get_image());
            image_views.push_back(image->get_image_view());
        }

        return Result::eSuccess;
    }

    Result VulkanRenderTarget::destroy() {
        component_destroy()

        for (auto &image : vulkan_images) {
            image->destroy();
        }

        this->images.clear();
        this->image_views.clear();

        return Result::eSuccess;
    }
}
