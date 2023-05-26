#pragma once

#include "vulkan_commons.hpp"
#include "renderpass.hpp"
#include "resource/vulkan_render_target.hpp"
#include "vulkan_command.hpp"

namespace MangoRHI {
    class VulkanSubpass {
    public:
        void set_name(const char *name);
        VulkanSubpass() = default;
        VulkanSubpass(VulkanSubpass &&other);
    
    define_readonly_pointer(char, name, "")
    define_extern_writeable_member(VkSubpassDescription, description, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(STL_IMPL::vector<VkAttachmentReference>, input_attachment, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(STL_IMPL::vector<VkAttachmentReference>, output_attachment, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(STL_IMPL::vector<VkAttachmentReference>, preserve_attachment, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VkAttachmentReference, depth_attachment, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VkAttachmentReference, resolve_attachment, MANGO_NO_INIT_VAULE)
    };

    class VulkanRenderPass final : public RenderPass {
        using RenderTargetMap = STL_IMPL::vector<STL_IMPL::pair<const char *, VulkanRenderTarget *>>;
    public:
        void attach_render_target(RenderTarget *render_target);
        
        void add_input_render_target(const char *render_target_name, RenderTargetLayout ref_layout) override;
        void add_output_render_target(const char *render_target_name, RenderTargetLayout ref_layout) override;
        void add_preserve_render_target(const char *render_target_name, RenderTargetLayout ref_layout) override;
        void set_depth_render_target(const char *render_target_name, RenderTargetLayout ref_layout) override;
        void set_resolve_render_target(const char *render_target_name, RenderTargetLayout ref_layout) override;
        void add_subpass(const char *subpass_name, PipelineBindPoint bind_point) override;
        void add_dependency(SubpassStageInfo src_subpass_name, SubpassStageInfo dst_subpass_name) override;

        Result create() override;
        Result destroy() override;

        Result begin_render_pass(VulkanCommand *commnad);
        Result end_render_pass(VulkanCommand *commnad);
    private:
        u32 get_render_target_index_by_name(const char *render_target_name);
        u32 get_subpass_index_by_name(const char *subpass_name);
        VkAttachmentReference get_render_target_ref(const char *render_target_name, RenderTargetLayout ref_layout);

    define_private_member(VulkanSubpass, temp_subpass, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkClearValue>, clear_values, MANGO_NO_INIT_VAULE)
    define_private_member(RenderTargetMap, render_targets, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanSubpass>, subpasses, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VkSubpassDependency>, dependencies, MANGO_NO_INIT_VAULE)
    define_member(VkRenderPass, render_pass, VK_NULL_HANDLE)
    };
}
