#include "vulkan_sampler.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanSampler::set_address_mode(SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w) {
        this->address_mode_u = sampler_address_mode2vk_sampler_address_mode(u);
        this->address_mode_v = sampler_address_mode2vk_sampler_address_mode(v);
        this->address_mode_w = sampler_address_mode2vk_sampler_address_mode(w);
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
        sampler_create_info.minLod = 0.0f;
        sampler_create_info.maxLod = static_cast<float>(mipmap_levels);
        sampler_create_info.mipLodBias = 0.0f;
        VK_CHECK(vkCreateSampler(vulkan_context->get_device()->get_logical_device(), &sampler_create_info, vulkan_context->get_allocator(), &sampler))
        RHI_DEBUG("Create vulkan sampler -> 0x{:x}", (AddrType)sampler)

        return Result::eSuccess;
    }

    Result VulkanSampler::destroy() {
        component_destroy()

        VK_CHECK(vkDeviceWaitIdle(vulkan_context->get_device()->get_logical_device()))

        RHI_DEBUG("Create vulkan sampler -> 0x{:x}", (AddrType)sampler)
        vkDestroySampler(vulkan_context->get_device()->get_logical_device(), sampler, vulkan_context->get_allocator());

        component_destroy_end()
    }
}
