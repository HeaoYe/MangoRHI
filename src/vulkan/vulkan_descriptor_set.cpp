#include "vulkan_descriptor_set.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    STL_IMPL::unordered_map<VkDescriptorType, u32> g_descriptor_info = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0 },
    };
    STL_IMPL::vector<VulkanDescriptorSet *> g_vulkan_descriptor_sets(0);

    Result VulkanUniformDescriptor::create() {
        component_create()

        buffer.set_size(1024);
        buffer.get_usage() = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        buffer.get_properties() = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        buffer.create();
        
        return Result::eSuccess;
    }

    Result VulkanUniformDescriptor::destroy() {
        component_destroy()

        buffer.destroy();

        return Result::eSuccess;
    }

    void VulkanUniformDescriptor::update(VulkanDescriptorSet *descriptor_set) {
        VkDescriptorBufferInfo descriptor_buffer_info {
            .buffer = buffer.get_buffer(),
            .offset = 0,
            .range = sizeof(float),
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_descriptor_set.descriptorCount = count;
        write_descriptor_set.dstSet = descriptor_set->get_descriptor_set();
        write_descriptor_set.dstBinding = n_binding;
        write_descriptor_set.dstArrayElement = 0;
        vkUpdateDescriptorSets(vulkan_context->get_device().get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
    }

    void VulkanDescriptorSet::add_descriptor(DescriptorUsage usage, DescriptorStage stage, u32 count) {
        switch (usage) {
        case DescriptorUsage::eUniform: {
            auto *uniform = new VulkanUniformDescriptor();
            descriptors.push_back(uniform);
            uniform->get_count() = count;
            uniform->get_n_binding() = _current_binding;
            auto &binding = uniform->get_binding();
            binding.binding = _current_binding;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.descriptorCount = count;
            binding.stageFlags = descriptor_stage2vk_shader_stage_flags(stage);
            binding.pImmutableSamplers = nullptr;
            g_descriptor_info.at(binding.descriptorType)++;
        } break;
        }
        _current_binding++;
    }

    VulkanDescriptorSet::VulkanDescriptorSet() {
        g_vulkan_descriptor_sets.push_back(this);
    }

    Result VulkanDescriptorSet::create() {
        component_create()

        STL_IMPL::vector<VkDescriptorSetLayoutBinding> bindings(descriptors.size());
        for (u32 index = 0; index < descriptors.size(); index++) {
            descriptors[index]->create();
            bindings[index] = descriptors[index]->get_binding();
        }
        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        descriptor_set_layout_create_info.pBindings = bindings.data();
        descriptor_set_layout_create_info.bindingCount = bindings.size();
        VK_CHECK(vkCreateDescriptorSetLayout(vulkan_context->get_device().get_logical_device(), &descriptor_set_layout_create_info, vulkan_context->get_allocator(), &layout))
        RHI_DEBUG("Create vulkan descriptor set layout -> 0x{:x}", (AddrType)layout)

        return Result::eSuccess;
    }

    Result VulkanDescriptorSet::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy vulkan descriptor set layout -> 0x{:x}", (AddrType)layout)
        vkDestroyDescriptorSetLayout(vulkan_context->get_device().get_logical_device(), layout, vulkan_context->get_allocator());

        for (auto &descriptor : descriptors) {
            descriptor->destroy();
        }

        return Result::eSuccess;
    }

    void VulkanDescriptorSet::update() {
        for (auto &descriptor : descriptors) {
            descriptor->update(this);
        }
    }
}
