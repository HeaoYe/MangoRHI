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

        VK_CHECK(vkDeviceWaitIdle(vulkan_context->get_device()->get_logical_device()))

        switch (usage) {
        case RenderTargetUsage::eColor:
            description.format = vulkan_context->get_swapchain()->get_format().format;
            description.samples = vulkan_context->get_multisample_count();
            description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        case RenderTargetUsage::eDepth:
            description.format = vulkan_context->get_depth_format();
            description.samples = vulkan_context->get_multisample_count();
            description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        case RenderTargetUsage::eColorBuffer:
            description.format = vulkan_context->get_swapchain()->get_format().format;
            description.samples = vulkan_context->get_multisample_count();
            description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
        if (strcmp(name, MANGORHI_SURFACE_RENDER_TARGET_NAME) == 0) {
            description.samples = VK_SAMPLE_COUNT_1_BIT;
            description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        if (images.size() == 0) {
            if (is_each_frame_render_target == MG_TRUE) {
                vulkan_images.resize(vulkan_context->get_swapchain()->get_image_count());
            } else {
                vulkan_images.resize(1);
            }
        }
        for (auto &image : vulkan_images) {
            image = std::make_unique<VulkanImage>();
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
            case RenderTargetUsage::eColorBuffer:
                image->set_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
                image->set_aspect(VK_IMAGE_ASPECT_COLOR_BIT);
                break;
            }
            if (is_resolve == MG_TRUE) {
                image->set_multisample_count(VK_SAMPLE_COUNT_1_BIT);
                description.samples = VK_SAMPLE_COUNT_1_BIT;
                description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            } else {
                image->set_multisample_count(vulkan_context->get_multisample_count());
            }
            image->create();
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
        this->vulkan_images.clear();

        this->images.clear();
        this->image_views.clear();

        return Result::eSuccess;
    }
}
