#include "vulkan_framebuffer.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanFrameBuffer::create() {
        component_create()

        VkFramebufferCreateInfo framebuffer_create_info { .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        framebuffer_create_info.renderPass = vulkan_context->get_render_pass().get_render_pass();
        framebuffer_create_info.width = vulkan_context->get_extent().width;
        framebuffer_create_info.height = vulkan_context->get_extent().height;
        framebuffer_create_info.layers = 1;
        framebuffers.resize(vulkan_context->get_swapchain().get_image_count());
        for (u32 index = 0; index < vulkan_context->get_swapchain().get_image_count(); index++) {
            STL_IMPL::vector<VkImageView> attachments;
            for (const auto &render_target : vulkan_context->get_render_pass().get_render_targets()) {
                if (render_target->get_is_each_frame_render_target() == MG_TRUE) {
                    attachments.push_back(render_target->get_image_views()[index]);
                } else {
                    attachments.push_back(render_target->get_image_views()[0]);
                }
            }
            framebuffer_create_info.pAttachments = attachments.data();
            framebuffer_create_info.attachmentCount = attachments.size();
            VK_CHECK(vkCreateFramebuffer(vulkan_context->get_device().get_logical_device(), &framebuffer_create_info, vulkan_context->get_allocator(), &framebuffers[index]))
            RHI_DEBUG("Create vulkan framebuffer({}) -> 0x{:x}", index, (AddrType)framebuffers[index])
        }

        return Result::eSuccess;
    }

    Result VulkanFrameBuffer::destroy() {
        component_destroy()

        for (u32 index = 0; index < vulkan_context->get_swapchain().get_image_count(); index++) {
            RHI_DEBUG("Destroy vulkan framebuffer({}) -> 0x{:x}", index, (AddrType)framebuffers[index])
            vkDestroyFramebuffer(vulkan_context->get_device().get_logical_device(), framebuffers[index], vulkan_context->get_allocator());
        }

        return Result::eSuccess;
    }
}
