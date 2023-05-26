#include "vulkan_render_target.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanRenderTarget::set_clear_color(ClearValue clear_value) {
        this->clear_value = clear_value2vk_clear_value(clear_value);
    }

    void VulkanRenderTarget::set_name(const char *name) {
        this->name = name;
    }

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
            break;
        case RenderTargetUsage::eDepth:
            description.format = VK_FORMAT_D32_SFLOAT;
            description.samples = VK_SAMPLE_COUNT_1_BIT;
            description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        }
    }

    void VulkanRenderTarget::add_render_target_data(VkImage image, VkImageView image_view) {
        this->images.push_back(image);
        this->image_views.push_back(image_view);
    }

    Result VulkanRenderTarget::create() {
        component_create()

        return Result::eSuccess;
    }

    Result VulkanRenderTarget::destroy() {
        component_destroy()

        this->images.clear();
        this->image_views.clear();

        return Result::eSuccess;
    }
}
