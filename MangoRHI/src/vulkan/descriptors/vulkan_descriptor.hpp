#pragma once

#include "../vulkan_commons.hpp"

namespace MangoRHI {
    class VulkanDescriptorSet;

    VkDescriptorType descriptor_type2vk_descriptor_type(DescriptorType type);

    struct VulkanDescriptorBinding {
        DescriptorStageFlags stages;
        DescriptorType type;
        u32 binding_index;
        u32 count;
        u32 data[3];
    };

    class VulkanDescriptor : public RuntimeComponent {
    public:
        virtual ~VulkanDescriptor() = default;
        virtual void update(const VulkanDescriptorSet &descriptor_set) = 0;
        virtual DescriptorType get_type() const = 0;

    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, binding_index, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, count, MANGO_NO_INIT_VAULE)
    };

    template<typename T>
    struct Ref {
        T *data;

        Ref(const T& data) : data(&data) {}
        Ref() : data(nullptr) {}
        Ref(const Ref &other) : data(other.data) {}
        Ref &operator=(const Ref &other) { data = other.data; return *this; }
        Ref(Ref &&other) : data(other.data) { other.data = nullptr; }
        Ref &operator=(Ref &&other) { data = other.data; other.data = nullptr; return *this; }
        Bool is_null() { return data == nullptr ? MG_TRUE : MG_FALSE; }
        operator T&() { return *data; }
        T *operator ->() { return data; }
    };
}
