#include "vulkan_sampler.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanSampler::set_mag_filter(SamplerFilter filter) {
        this->mag_filter = smapler_filter2vk_filter(filter);
    }

    void VulkanSampler::set_min_filter(SamplerFilter filter) {
        this->min_filter = smapler_filter2vk_filter(filter);
    }

    void VulkanSampler::set_address_mode(SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w) {
        this->address_mode_u = sampler_address_mode2vk_sampler_address_mode(u);
        this->address_mode_v = sampler_address_mode2vk_sampler_address_mode(v);
        this->address_mode_w = sampler_address_mode2vk_sampler_address_mode(w);
    }

    void VulkanSampler::set_max_anisotropy(u32 anisotropy) {
        this->max_anisotropy = anisotropy;
    }

    void VulkanSampler::set_border_color(SamplerBorderColor color) {
        this->border_color = sampler_border_color2vk_border_color(color);
    }

    void VulkanSampler::set_mipmap_mode(SamplerFilter mipmap) {
        this->mipmap_mode = sampler_filter2vk_sampler_mipmap_mode(mipmap);
    }

    Result VulkanSampler::create() {
        component_create()

        VkSamplerCreateInfo sampler_create_info { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        sampler_create_info.magFilter = mag_filter;
        sampler_create_info.minFilter = min_filter;
        sampler_create_info.addressModeU = address_mode_u;
        sampler_create_info.addressModeV = address_mode_v;
        sampler_create_info.addressModeW = address_mode_w;
        sampler_create_info.anisotropyEnable = VK_TRUE;
        sampler_create_info.maxAnisotropy = max_anisotropy;
        sampler_create_info.borderColor = border_color;
        sampler_create_info.unnormalizedCoordinates = VK_FALSE;
        sampler_create_info.compareEnable = VK_FALSE;
        sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_create_info.mipmapMode = mipmap_mode;
        sampler_create_info.mipLodBias = 0.0f;
        sampler_create_info.minLod = 0.0f;
        sampler_create_info.maxLod = 0.0f;
        VK_CHECK(vkCreateSampler(vulkan_context->get_device().get_logical_device(), &sampler_create_info, vulkan_context->get_allocator(), &sampler))
        RHI_DEBUG("Create vulkan sampler -> 0x{:x}", (AddrType)sampler)

        return Result::eSuccess;
    }

    Result VulkanSampler::destroy() {
        component_destroy()
        
        RHI_DEBUG("Create vulkan sampler -> 0x{:x}", (AddrType)sampler)
        vkDestroySampler(vulkan_context->get_device().get_logical_device(), sampler, vulkan_context->get_allocator());

        return Result::eSuccess;
    }
}
