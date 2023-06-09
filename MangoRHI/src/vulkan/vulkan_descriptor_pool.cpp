#include "vulkan_descriptor_pool.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanDescriptorPool::create() {
        component_create()

        u32 size = g_vulkan_descriptor_sets.size();
        if (size == 0) {
            RHI_DEBUG("No vulkan descriptor pool has been created")
            RHI_DEBUG("No vulkan descriptor set has been allocated")
            return Result::eSuccess;
        }

        STL_IMPL::vector<VkDescriptorPoolSize> pool_sizes;
        for (const auto &descriptor_info : g_descriptor_info) {
            if (descriptor_info.second > 0) {
                pool_sizes.push_back(VkDescriptorPoolSize {
                    .type = descriptor_info.first,
                    .descriptorCount = descriptor_info.second * vulkan_context->get_max_in_flight_frame_count(),
                });
            }
        }

        VkDescriptorPoolCreateInfo descriptor_pool_create_info { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        descriptor_pool_create_info.pPoolSizes = pool_sizes.data();
        descriptor_pool_create_info.poolSizeCount = pool_sizes.size();
        descriptor_pool_create_info.maxSets = g_vulkan_descriptor_sets.size() * vulkan_context->get_max_in_flight_frame_count();
        VK_CHECK(vkCreateDescriptorPool(vulkan_context->get_device()->get_logical_device(), &descriptor_pool_create_info, vulkan_context->get_allocator(), &descriptor_pool))
        RHI_DEBUG("Create vulkan descriptor pool -> 0x{:x}", (AddrType)descriptor_pool)

        STL_IMPL::vector<VkDescriptorSetLayout> layouts(size);
        STL_IMPL::vector<STL_IMPL::vector<VkDescriptorSet>> sets(vulkan_context->get_max_in_flight_frame_count());
        for (u32 index = 0; index < g_vulkan_descriptor_sets.size(); index++) {
            g_vulkan_descriptor_sets[index]->create();
            layouts[index] = g_vulkan_descriptor_sets[index]->get_layout();
        }
        VkDescriptorSetAllocateInfo descriptor_set_allocate_info { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        descriptor_set_allocate_info.descriptorPool = descriptor_pool;
        descriptor_set_allocate_info.descriptorSetCount = size;
        descriptor_set_allocate_info.pSetLayouts = layouts.data();
        for (auto &flight : sets) {
            flight.resize(size);
            VK_CHECK(vkAllocateDescriptorSets(vulkan_context->get_device()->get_logical_device(), &descriptor_set_allocate_info, flight.data()))
        }
        RHI_DEBUG("Allocate {} vulkan descriptor set", sets.size() * size)
        for (u32 index = 0; index < g_vulkan_descriptor_sets.size(); index++) {
            for (u32 in_flight_index = 0; in_flight_index < vulkan_context->get_max_in_flight_frame_count(); in_flight_index++) {
                g_vulkan_descriptor_sets[index]->in_flight_descriptor_sets[in_flight_index] = sets[in_flight_index][index];
            }
            g_vulkan_descriptor_sets[index]->update();
        }

        return Result::eSuccess;
    }

    Result VulkanDescriptorPool::destroy() {
        component_destroy()

        if (descriptor_pool == VK_NULL_HANDLE) {
            return Result::eSuccess;
        }

        RHI_DEBUG("Destroy vulkan descriptor pool -> 0x{:x}", (AddrType)descriptor_pool)
        vkDestroyDescriptorPool(vulkan_context->get_device()->get_logical_device(), descriptor_pool, vulkan_context->get_allocator());

        for (auto &descriptor_set : g_vulkan_descriptor_sets) {
            descriptor_set->destroy();
        }

        return Result::eSuccess;
    }
}
