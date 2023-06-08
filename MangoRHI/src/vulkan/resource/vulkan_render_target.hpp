#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/render_target.hpp"
#include "vulkan_image.hpp"

namespace MangoRHI {
    class VulkanRenderTarget final : public RenderTarget {
    public:
        void add_render_target_data(VkImage image, VkImageView image_view);

    define_readonly_pointer(MANGO_CONST_GETTER, MANGO_SETTER_OVERRIDE, char, name, "")
    define_member_with_translator(MANGO_CONST_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, ClearValue, VkClearValue, clear_color, clear_value2vk_clear_value, ClearValue {})
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC_OVERRIDE, RenderTargetUsage, usage, MANGO_NO_INIT_VAULE)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC, u32, index, MANGO_NO_INIT_VAULE)

    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, VkAttachmentDescription, description, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanImage *>, vulkan_images, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkImage>, images, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkImageView>, image_views, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, Bool, is_each_frame_render_target, MG_FALSE)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC, Bool, is_resolve, MG_FALSE)

    declare_component_cls(VulkanRenderTarget)
    };
}
