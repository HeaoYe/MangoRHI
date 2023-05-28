#include "vulkan_descriptor_set.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    STL_IMPL::unordered_map<VkDescriptorType, u32> g_descriptor_info = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0 }
    };
    STL_IMPL::vector<VulkanDescriptorSet *> g_vulkan_descriptor_sets(0);

    Result VulkanUniformDescriptor::create() {
        component_create()

        u32 align = 0x40;
        while (align < size) {
            align *= 2;
        }
        size = align;

        buffer.set_size(size * get_count());
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
        STL_IMPL::vector<VkDescriptorBufferInfo> descriptor_buffer_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            descriptor_buffer_infos[index].buffer = buffer.get_buffer();
            descriptor_buffer_infos[index].offset = index * size;
            descriptor_buffer_infos[index].range = size;
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pBufferInfo = descriptor_buffer_infos.data();
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstSet = descriptor_set->get_descriptor_set();
        write_descriptor_set.dstBinding = get_n_binding();
        write_descriptor_set.dstArrayElement = 0;
        vkUpdateDescriptorSets(vulkan_context->get_device().get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
    }

    void VulkanTextureDescriptor::add_texture(VulkanTexture *textures) {
        this->textures.push_back(textures);
    }

    void VulkanTextureDescriptor::set_texture(u32 index, VulkanTexture *texture) {
        this->textures[index] = texture;
    }

    Result VulkanTextureDescriptor::create() {
        component_create()

        return Result::eSuccess;
    }

    Result VulkanTextureDescriptor::destroy() {
        component_destroy()

        return Result::eSuccess;
    }

    void VulkanTextureDescriptor::update(VulkanDescriptorSet *descriptor_set) {
        STL_IMPL::vector<VkDescriptorImageInfo> descriptor_image_infos(get_count());
        for (u32 index = 0; index < get_count(); index++) {
            descriptor_image_infos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptor_image_infos[index].imageView = textures[index]->get_image().get_image_view();
            descriptor_image_infos[index].sampler = textures[index]->get_sampler()->get_sampler();
        };
        VkWriteDescriptorSet write_descriptor_set { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write_descriptor_set.pImageInfo = descriptor_image_infos.data();
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_descriptor_set.descriptorCount = get_count();
        write_descriptor_set.dstSet = descriptor_set->get_descriptor_set();
        write_descriptor_set.dstBinding = get_n_binding();
        write_descriptor_set.dstArrayElement = 0;
        vkUpdateDescriptorSets(vulkan_context->get_device().get_logical_device(), 1, &write_descriptor_set, 0, nullptr);
    }

    void VulkanDescriptorSet::add_uniform(DescriptorStage stage, u32 size, u32 count) {
        auto *uniform_descriptor = new VulkanUniformDescriptor();
        descriptors.push_back(uniform_descriptor);
        uniform_descriptor->get_size() = size;
        setup_descriptor_binding(uniform_descriptor, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, stage, count);
    }

    void VulkanDescriptorSet::add_textures(DescriptorStage stage, Texture **textures, u32 count) {
        auto *texture_descriptor = new VulkanTextureDescriptor();
        for (u32 index = 0; index < count; index++) {
            texture_descriptor->add_texture((VulkanTexture *)textures[index]);
        }
        descriptors.push_back(texture_descriptor);
        setup_descriptor_binding(texture_descriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage, count);
    }

    VkDescriptorSetLayoutBinding &VulkanDescriptorSet::setup_descriptor_binding(VulkanDescriptor *descriptor, VkDescriptorType type, DescriptorStage stage, u32 count) {
        descriptor->get_n_binding() = _current_binding;
        descriptor->get_count() = count;
        auto &binding = descriptor->get_binding();
        binding.binding = _current_binding;
        binding.descriptorType = type;
        binding.descriptorCount = count;
        binding.stageFlags = descriptor_stage2vk_shader_stage_flags(stage);
        binding.pImmutableSamplers = nullptr;
        _current_binding++;
        g_descriptor_info[binding.descriptorType]++;
        return binding;
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

    void *VulkanDescriptorSet::map_uniform_buffer_pointer(u32 binding, u32 index) {
        auto *descriptor = (VulkanUniformDescriptor *)descriptors[binding];
        return (void*)(((u8 *)descriptor->get_buffer().map()) + index * descriptor->get_size());
    }

    void VulkanDescriptorSet::set_texture(u32 binding, u32 index, Texture *texture) {
        auto *descriptor = (VulkanTextureDescriptor *)descriptors[binding];
        descriptor->set_texture(index, (VulkanTexture *)texture);
    }

    void VulkanDescriptorSet::update() {
        for (auto &descriptor : descriptors) {
            descriptor->update(this);
        }
    }
}
