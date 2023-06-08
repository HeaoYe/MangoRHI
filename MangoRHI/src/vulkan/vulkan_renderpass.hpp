#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/renderpass.hpp"
#include "resource/vulkan_render_target.hpp"
#include "vulkan_command.hpp"

namespace MangoRHI {
    class VulkanSubpass {
    public:
        VulkanSubpass() = default;
        void build(const char *name, PipelineBindPoint bind_point, u32 index);

    define_readonly_pointer(MANGO_CONST_GETTER, MANGO_NO_SETTER, char, name, "")
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkSubpassDescription, description, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkPipelineBindPoint, bind_point, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkAttachmentReference>, input_attachments, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkAttachmentReference>, output_attachments, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkPipelineColorBlendAttachmentState>, color_blend_states, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<u32>, preserve_attachments, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, STL_IMPL::optional<VkAttachmentReference>, depth_attachment, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkAttachmentReference>, resolve_attachment, MANGO_NO_INIT_VAULE)

    private:
        VulkanSubpass(const VulkanSubpass &) = delete;
        VulkanSubpass &operator=(const VulkanSubpass &) = delete;
        VulkanSubpass(VulkanSubpass &&) = delete;
        VulkanSubpass &operator=(VulkanSubpass &&) = delete;
    };

    class VulkanRenderPass final : public RenderPass {
    public:
        void attach_render_target(RenderTarget &render_target);

        void add_input_render_target(const char *render_target_name, RenderTargetLayout ref_layout) override;
        void add_output_render_target(const char *render_target_name, RenderTargetLayout ref_layout, ColorBlendInfo color_blend_info) override;
        void add_preserve_render_target(const char *render_target_name) override;
        void set_depth_render_target(const char *render_target_name, RenderTargetLayout ref_layout) override;
        void add_resolve_render_target(const char *render_target_name, RenderTargetLayout ref_layout) override;
        void add_subpass(const char *subpass_name, PipelineBindPoint bind_point) override;
        void add_dependency(SubpassStageInfo src_subpass_name, SubpassStageInfo dst_subpass_name) override;

        Result begin_render_pass(VulkanCommand &commnad);
        Result end_render_pass(VulkanCommand &commnad);

        u32 get_subpass_index_by_name(const char *subpass_name) const;
        u32 get_render_target_index_by_name(const char *render_target_name) const;
    private:
        VkAttachmentReference get_render_target_ref(const char *render_target_name, RenderTargetLayout ref_layout) const;

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<Reference<VulkanRenderTarget>>, render_targets, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkClearValue>, clear_values, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<Reference<VulkanSubpass>>, subpasses, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkSubpassDependency>, dependencies, MANGO_NO_INIT_VAULE)
    define_private_pointer(VulkanSubpass, temp_subpass, new VulkanSubpass())

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkRenderPass, render_pass, VK_NULL_HANDLE)

    declare_component_cls(VulkanRenderPass)
    };
}
