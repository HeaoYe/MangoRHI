#include "vulkan_descriptor.hpp"

namespace MangoRHI {
    VkDescriptorType descriptor_type2vk_descriptor_type(DescriptorType type) {
        switch (type) {
        case DescriptorType::eNone:
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
        case DescriptorType::eUniform:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case DescriptorType::eTexture:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case DescriptorType::eInputRenderTarget:
            return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        }
    }
}
