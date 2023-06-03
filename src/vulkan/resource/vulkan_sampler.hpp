#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/sampler.hpp"

namespace MangoRHI {
    class VulkanSampler final : public Sampler {
    public:
        void set_address_mode(SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w) override;

        Result create() override;
        Result destroy() override;
    
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, SamplerFilter, VkFilter, mag_filter, sampler_filter2vk_filter, SamplerFilter::eLinear)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, SamplerFilter, VkFilter, min_filter, sampler_filter2vk_filter, SamplerFilter::eLinear)
    define_private_member(VkSamplerAddressMode, address_mode_u, VK_SAMPLER_ADDRESS_MODE_REPEAT)
    define_private_member(VkSamplerAddressMode, address_mode_v, VK_SAMPLER_ADDRESS_MODE_REPEAT)
    define_private_member(VkSamplerAddressMode, address_mode_w, VK_SAMPLER_ADDRESS_MODE_REPEAT)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC_OVERRIDE, u32, max_anisotropy, 1)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, SamplerBorderColor, VkBorderColor, border_color, sampler_border_color2vk_border_color, SamplerBorderColor::eIntOpaqueBlack)
    define_member_with_translator(MANGO_NO_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, SamplerFilter, VkSamplerMipmapMode, mipmap_mode, sampler_filter2vk_sampler_mipmap_mode, SamplerFilter::eLinear)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC, u32, mipmap_levels, MANGO_NO_INIT_VAULE)

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkSampler, sampler, VK_NULL_HANDLE)

    declare_component_cls(VulkanSampler)
    };
}
