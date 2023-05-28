#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/sampler.hpp"

namespace MangoRHI {
    class VulkanSampler final : public Sampler {
    public:
        void set_mag_filter(SamplerFilter filter) override;
        void set_min_filter(SamplerFilter filter) override;
        void set_address_mode(SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w) override;
        void set_max_anisotropy(u32 anisotropy) override;
        void set_border_color(SamplerBorderColor color) override;
        void set_mipmap_mode(SamplerFilter mipmap) override;

        Result create() override;
        Result destroy() override;
    
    define_member(VkSampler, sampler, VK_NULL_HANDLE)
    define_private_member(VkFilter, mag_filter, VK_FILTER_LINEAR)
    define_private_member(VkFilter, min_filter, VK_FILTER_LINEAR)
    define_private_member(VkSamplerAddressMode, address_mode_u, VK_SAMPLER_ADDRESS_MODE_REPEAT)
    define_private_member(VkSamplerAddressMode, address_mode_v, VK_SAMPLER_ADDRESS_MODE_REPEAT)
    define_private_member(VkSamplerAddressMode, address_mode_w, VK_SAMPLER_ADDRESS_MODE_REPEAT)
    define_private_member(u32, max_anisotropy, 1)
    define_private_member(VkBorderColor, border_color, VK_BORDER_COLOR_INT_OPAQUE_BLACK)
    define_private_member(VkSamplerMipmapMode, mipmap_mode, VK_SAMPLER_MIPMAP_MODE_LINEAR)

    no_copy_and_move_construction(VulkanSampler)
    };
}
