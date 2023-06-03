#include "vulkan_render_target.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanRenderTarget::add_render_target_data(VkImage image, VkImageView image_view) {
        this->images.push_back(image);
        this->image_views.push_back(image_view);
        if (images.size() > 1) {
            is_each_frame_render_target = MG_TRUE;
        }
    }

    Result VulkanRenderTarget::create() {
        component_create()

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
            break;
        case RenderTargetUsage::eDepth:
            description.format = vulkan_context->get_depth_format();
            description.samples = VK_SAMPLE_COUNT_1_BIT;
            description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        }

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
            switch (usage) {
            case RenderTargetUsage::eColor:
                image->set_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
                image->set_aspect(VK_IMAGE_ASPECT_COLOR_BIT);
                break;
            case RenderTargetUsage::eDepth:
                image->set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
                image->set_aspect(VK_IMAGE_ASPECT_DEPTH_BIT);
                break;
            }
            image->create();
            vulkan_context->transition_image_layout(image->get_image(), image->get_format(), VK_IMAGE_LAYOUT_UNDEFINED, description.finalLayout, 1);
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
