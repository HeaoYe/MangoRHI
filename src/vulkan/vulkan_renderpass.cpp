#include "vulkan_renderpass.hpp"
#include "resource/vulkan_render_target.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    void VulkanSubpass::set_name(const char *name) {
        this->name = name;
    }

    VulkanSubpass::VulkanSubpass(VulkanSubpass &&other) {
        this->name = other.name;
        other.name = nullptr;
        this->description = other.description;
        other.description = VkSubpassDescription {};
        this->input_attachment = STL_IMPL::move(other.input_attachment);
        this->output_attachment = STL_IMPL::move(other.output_attachment);
        this->preserve_attachment = STL_IMPL::move(other.preserve_attachment);
        this->depth_attachment = other.depth_attachment;
        other.depth_attachment = VkAttachmentReference {};
        this->resolve_attachment = other.resolve_attachment;
        other.resolve_attachment = VkAttachmentReference {};
    }

    u32 VulkanRenderPass::get_render_target_index_by_name(const char *render_target_name) {
        if (strcmp(render_target_name, MANGORHI_SURFACE_RENDER_TARGET_NAME) == 0) {
            return 0;
        }
        u32 index = 1;
        for (const auto &render_target_pair: render_targets) {
            if (strcmp(render_target_pair.first, render_target_name) == 0) {
                break;
            }
            index++;
        }
        return index;
    }

    VkAttachmentReference VulkanRenderPass::get_render_target_ref(const char *render_target_name, RenderTargetLayout ref_layout) {
        u32 attachment = get_render_target_index_by_name(render_target_name);
        if (attachment == render_targets.size()) {
            RHI_ERROR("RenderTarget {} not found", render_target_name)
        }
        return VkAttachmentReference {
            .attachment = attachment,
            .layout = render_target_layout2vk_image_layout(ref_layout),
        };
    }

    u32 VulkanRenderPass::get_subpass_index_by_name(const char *subpass_name) {
        if (strcmp(subpass_name, MANGORHI_EXTERNAL_SUBPASS_NAME) == 0) {
            return VK_SUBPASS_EXTERNAL;
        }
        u32 index = 0;
        for (const auto &subpass : subpasses) {
            if (strcmp(subpass.get_name(), subpass_name) == 0) {
                break;
            }
            index++;
        }
        return index;
    }

    void VulkanRenderPass::attach_render_target(RenderTarget *render_target) {
        VulkanRenderTarget *vulkan_render_target = (VulkanRenderTarget *)render_target;
        vulkan_render_target->get_index() = render_targets.size() + 1;
        if (get_render_target_index_by_name(vulkan_render_target->get_name()) < render_targets.size()) {
            RHI_ERROR("RenderTarget {} is existed", vulkan_render_target->get_name());
        }
        render_targets.push_back(STL_IMPL::make_pair(vulkan_render_target->get_name(), vulkan_render_target));
    }

    void VulkanRenderPass::add_input_render_target(const char *render_target_name, RenderTargetLayout ref_layout) {
        temp_subpass.get_input_attachment().push_back(get_render_target_ref(render_target_name, ref_layout));
    }

    void VulkanRenderPass::add_output_render_target(const char *render_target_name, RenderTargetLayout ref_layout) {
        temp_subpass.get_output_attachment().push_back(get_render_target_ref(render_target_name, ref_layout));
    }

    void VulkanRenderPass::add_preserve_render_target(const char *render_target_name, RenderTargetLayout ref_layout) {
        temp_subpass.get_preserve_attachment().push_back(get_render_target_ref(render_target_name, ref_layout));
    }

    void VulkanRenderPass::set_depth_render_target(const char *render_target_name, RenderTargetLayout ref_layout) {
        temp_subpass.get_depth_attachment() = get_render_target_ref(render_target_name, ref_layout);
        RHI_WARN("{}", temp_subpass.get_depth_attachment().attachment)
    }

    void VulkanRenderPass::set_resolve_render_target(const char *render_target_name, RenderTargetLayout ref_layout) {
        temp_subpass.get_resolve_attachment() = get_render_target_ref(render_target_name, ref_layout);
    }

    void VulkanRenderPass::add_subpass(const char *subpass_name, PipelineBindPoint bind_point) {
        temp_subpass.get_description().pipelineBindPoint = pipeline_bind_point2vk_pipeline_bind_point(bind_point);
        temp_subpass.set_name(subpass_name);
        u32 index = get_subpass_index_by_name(subpass_name);
        if (index == VK_SUBPASS_EXTERNAL || index < subpasses.size()) {
            RHI_ERROR("Subpass {} is existed", subpass_name)
        }
        subpasses.push_back(STL_IMPL::move(temp_subpass));
    }

    void VulkanRenderPass::add_dependency(SubpassStageInfo src_subpass_info, SubpassStageInfo dst_subpass_info) {
        VkSubpassDependency dependency {};
        dependency.srcSubpass = get_subpass_index_by_name(src_subpass_info.name);
        dependency.dstSubpass = get_subpass_index_by_name(dst_subpass_info.name);
        if (dependency.srcSubpass != VK_SUBPASS_EXTERNAL && dependency.srcSubpass == subpasses.size()) {
            RHI_ERROR("Unknown src subpass name {}", src_subpass_info.name)
        }
        if (dependency.dstSubpass != VK_SUBPASS_EXTERNAL && dependency.dstSubpass == subpasses.size()) {
            RHI_ERROR("Unknown dst subpass name {}", dst_subpass_info.name)
        }
        dependency.srcStageMask = pipeline_stage2vk_pipeline_stage_flags(src_subpass_info.stage);
        dependency.dstStageMask = pipeline_stage2vk_pipeline_stage_flags(dst_subpass_info.stage);
        dependency.srcAccessMask = access2vk_access_flags(src_subpass_info.access);
        dependency.dstAccessMask = access2vk_access_flags(dst_subpass_info.access);
    }

    Result VulkanRenderPass::create() {
        component_create()

        STL_IMPL::vector<VkAttachmentDescription> attachment_descriptions;
        STL_IMPL::vector<VkSubpassDescription> subpass_descriptions;
        attachment_descriptions.push_back(vulkan_context->get_swapchain().get_render_target().get_description());
        clear_values.push_back(vulkan_context->get_swapchain().get_render_target().get_clear_value());
        for (const auto &render_target_pair : render_targets) {
            attachment_descriptions.push_back(render_target_pair.second->get_description());
            clear_values.push_back(render_target_pair.second->get_clear_value());
        }
        for (const auto &subpass : subpasses) {
            subpass_descriptions.push_back(subpass.get_description());
        }

        VkRenderPassCreateInfo render_pass_create_info { .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        render_pass_create_info.pAttachments = attachment_descriptions.data();
        render_pass_create_info.attachmentCount = attachment_descriptions.size();
        render_pass_create_info.pSubpasses = subpass_descriptions.data();
        render_pass_create_info.subpassCount = subpass_descriptions.size();
        render_pass_create_info.pDependencies = dependencies.data();
        render_pass_create_info.dependencyCount = dependencies.size();

        VK_CHECK(vkCreateRenderPass(vulkan_context->get_device().get_logical_device(), &render_pass_create_info, vulkan_context->get_allocator(), &render_pass))
        RHI_DEBUG("Create vulkan render pass -> 0x{:x}", (AddrType)render_pass)

        return Result::eSuccess;
    }

    Result VulkanRenderPass::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy vulkan render pass -> 0x{:x}", (AddrType)render_pass)
        vkDestroyRenderPass(vulkan_context->get_device().get_logical_device(), render_pass, vulkan_context->get_allocator());

        return Result::eSuccess;
    }

    Result VulkanRenderPass::begin_render_pass(VulkanCommand *command) {
        VkRenderPassBeginInfo render_pass_begin_info { .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        render_pass_begin_info.renderPass = render_pass;
        render_pass_begin_info.renderArea = VkRect2D { .offset = { .x = 0, .y = 0}, .extent = vulkan_context->get_extent() };
        // render_pass_begin_info.framebuffer = framebuffer;
        render_pass_begin_info.pClearValues = clear_values.data();
        render_pass_begin_info.clearValueCount = clear_values.size();
        // vkCmdBeginRenderPass(command->get_command_buffer(), &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        return Result::eSuccess;
    }

    Result VulkanRenderPass::end_render_pass(VulkanCommand *commnad) {
        // vkCmdEndRenderPass(commnad->get_command_buffer());

        return Result::eSuccess;
    }
}
