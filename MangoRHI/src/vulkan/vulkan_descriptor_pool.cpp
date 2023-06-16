#include "vulkan_descriptor_pool.hpp"
#include "vulkan_context.hpp"
#include "descriptors/vulkan_uniform_descriptor.hpp"
#include "descriptors/vulkan_texture_descriptor.hpp"
#include "descriptors/vulkan_input_render_target_descriptor.hpp"

namespace MangoRHI {
    Result VulkanDescriptorPool::create() {
        component_create()

        STL_IMPL::vector<VkDescriptorPoolSize> pool_sizes = {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 },
        };

        VkDescriptorPoolCreateInfo descriptor_pool_create_info { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        descriptor_pool_create_info.pPoolSizes = pool_sizes.data();
        descriptor_pool_create_info.poolSizeCount = pool_sizes.size();
        descriptor_pool_create_info.maxSets = 500 * vulkan_context->get_max_in_flight_frame_count();
        VK_CHECK(vkCreateDescriptorPool(vulkan_context->get_device()->get_logical_device(), &descriptor_pool_create_info, vulkan_context->get_allocator(), &descriptor_pool))
        RHI_DEBUG("Create vulkan descriptor pool -> 0x{:x}", (AddrType)descriptor_pool)

        return Result::eSuccess;
    }

    void VulkanDescriptorPool::allocate(VulkanDescriptorSetLayout &layout, VulkanDescriptorSet &descriptor_set) const {
        descriptor_set.index = layout.get_index();
        descriptor_set.create();
        STL_IMPL::vector<VkDescriptorSetLayout> layouts(vulkan_context->get_max_in_flight_frame_count(), layout.get_layout());
        VkDescriptorSetAllocateInfo descriptor_set_allocate_info { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        descriptor_set_allocate_info.descriptorPool = descriptor_pool;
        descriptor_set_allocate_info.descriptorSetCount = layouts.size();
        descriptor_set_allocate_info.pSetLayouts = layouts.data();
        VK_CHECK(vkAllocateDescriptorSets(vulkan_context->get_device()->get_logical_device(), &descriptor_set_allocate_info, descriptor_set.in_flight_descriptor_sets.data()))
        RHI_DEBUG("Allocate vulkan descriptor set -> 0x{:x}", (AddrType)descriptor_set.in_flight_descriptor_sets[0]);

        for (auto &binding : layout.get_bindings()) {
            std::unique_ptr<VulkanDescriptor> descriptor;
            switch (binding.type) {
            case DescriptorType::eUniform: {
                auto *uniform_descriptor = new VulkanUniformDescriptor();
                uniform_descriptor->set_size(binding.data[0]);
                descriptor.reset(uniform_descriptor);
            } break;
            case MangoRHI::DescriptorType::eTexture:
                descriptor.reset(new VulkanTextureDescriptor());
                break;
            case MangoRHI::DescriptorType::eInputRenderTarget:
                descriptor.reset(new VulkanInputRenderTargetDescriptor());
                break;
            default:
                RHI_ERROR("Unknown vulkan descriptor type")
                return;
            }
            descriptor->set_binding_index(binding.binding_index);
            descriptor->set_count(binding.count);
            descriptor->create();
            descriptor_set.descriptors.push_back(std::move(descriptor));
        }
    }

    void VulkanDescriptorPool::free(VulkanDescriptorSet &descriptor_set) const {
        for (auto &descriptor : descriptor_set.descriptors) {
            descriptor->destroy();
        }
        descriptor_set.descriptors.clear();
        descriptor_set.destroy();
    }

    Result VulkanDescriptorPool::destroy() {
        component_destroy()

        if (descriptor_pool == VK_NULL_HANDLE) {
            return Result::eSuccess;
        }

        RHI_DEBUG("Destroy vulkan descriptor pool -> 0x{:x}", (AddrType)descriptor_pool)
        vkDestroyDescriptorPool(vulkan_context->get_device()->get_logical_device(), descriptor_pool, vulkan_context->get_allocator());

        component_destroy_end()
    }
}
