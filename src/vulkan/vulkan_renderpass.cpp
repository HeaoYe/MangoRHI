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

    VkAttachmentReference VulkanRenderPass::get_render_target_ref(const char *render_target_name, RenderTargetLayout ref_layout) {
        u32 attachment = 0;
        if (strcmp(render_target_name, MANGO_VULKAN_SURFACE_RENDER_TARGET_NAME) != 0) {
            attachment = render_targets.at(render_target_name)->get_index();
        }
        return VkAttachmentReference {
            .attachment = attachment,
            .layout = render_target_layout2vk_image_layout(ref_layout),
        };
    }

    void VulkanRenderPass::attach_render_target(RenderTarget *render_target) {
        VulkanRenderTarget *vulkan_render_target = (VulkanRenderTarget *)render_target;
        vulkan_render_target->get_index() = render_targets.size() + 1;
        render_targets.insert(STL_IMPL::make_pair(vulkan_render_target->get_name(), vulkan_render_target));
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
        subpasses.push_back(STL_IMPL::move(temp_subpass));
    }

    Result VulkanRenderPass::create() {
        component_create()

        STL_IMPL::vector<VkAttachmentDescription> attachment_descriptions;
        STL_IMPL::vector<VkSubpassDescription> subpass_descriptions;
        STL_IMPL::vector<VkSubpassDependency> dependencies;
        attachment_descriptions.push_back(((VulkanSwapchain *)vulkan_context->get_swapchain())->get_render_target().get_description());
        for (const auto &render_target : render_targets) {
            attachment_descriptions.push_back(render_target.second->get_description());
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
}
