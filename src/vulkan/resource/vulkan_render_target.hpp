#pragma once

#include "../vulkan_commons.hpp"
#include "resource/render_target.hpp"

namespace MangoRHI {
    class VulkanRenderTarget final : public RenderTarget {
    public:
        void set_clear_color(ClearValue clear_value) override;
        void set_name(const char *name) override;
        void set_usage(RenderTargetUsage usage) override;
        Result create() override;
        Result destroy() override;

        void add_render_target_data(VkImage image, VkImageView image_view);

    define_readonly_pointer(char, name, "")
    define_member(VkClearValue, clear_value, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VkImage>, images, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VkImageView>, image_views, MANGO_NO_INIT_VAULE)
    define_member(VkAttachmentDescription, description, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(u32, index, MANGO_NO_INIT_VAULE)
    define_member(Bool, is_each_frame_render_target, MG_FALSE)
    };
}
