#pragma once

#include "MangoRHI/context.hpp"
#include "vulkan_commons.hpp"
#include "vulkan_device.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_renderpass.hpp"
#include "vulkan_framebuffer.hpp"
#include "vulkan_synchronization.hpp"
#include "vulkan_command_pool.hpp"
#include "vulkan_descriptor_pool.hpp"
#include "vulkan_resource_factory.hpp"

namespace MangoRHI {
    using RecreateCallback = std::function<void(void)>;

    class VulkanContext final : public Context {
    public:
        void set_api_info(const void *info) override;
        void set_device_name(const char *name) override;
        void set_swapchain_image_count(u32 count) override;
        void set_clear_value(const char *render_target_name, ClearValue value) override;

        void resize(u32 width, u32 height) override;
        const u32 get_width() const override { return extent.width; }
        const u32 get_height() const override { return extent.height; }
        ResourceFactory &get_resource_factory_reference() override { return *resource_factory; }
        RenderPass &get_render_pass_reference() override { return *render_pass; }
        Command &get_current_command_reference() override { return *commands[current_in_flight_frame_index]; };

        Result begin_frame() override;
        Result end_frame() override;

        void add_resource_recreate_callback(RecreateCallback callback);

        VkFormat find_supported_format(const STL_IMPL::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
        VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect, uint32_t mipmap_levels) const;
        u32 find_memory_index(u32 type_filter, VkMemoryPropertyFlags flags) const;
        void transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mipmap_levels) const;

    private:
        void recreate_resources();

    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC_OVERRIDE, u32, max_in_flight_frame_count, 2)
    define_member_with_translator(MANGO_CONST_GETTER, MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE, MultisampleCount, VkSampleCountFlagBits, multisample_count, multisample_count2vk_sample_count, MultisampleCount::e1)
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC_OVERRIDE, Bool, vsync_enabled, MG_TRUE)

    define_pointer(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkAllocationCallbacks, allocator, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkInstance, instance, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkSurfaceKHR, surface, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, std::unique_ptr<VulkanDevice>, device, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, std::unique_ptr<VulkanSwapchain>, swapchain, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, std::unique_ptr<VulkanRenderPass>, render_pass, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, std::unique_ptr<VulkanFrameBuffer>, framebuffer, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, std::unique_ptr<VulkanDescriptorPool>, descriptor_pool, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, std::unique_ptr<VulkanSynchronization>, synchronization, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, std::unique_ptr<VulkanCommandPool>, command_pool, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, VkExtent2D, extent, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkFormat, depth_format, VK_FORMAT_UNDEFINED)
    define_member(MANGO_NO_GETTER, MANGO_NO_SETTER, VkSampleCountFlagBits, max_multisample_count, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, u32, current_in_flight_frame_index, 0)
    define_private_readonly_pointer(VulkanContextInfo, info, MANGO_NO_INIT_VAULE)
    define_private_member(std::unique_ptr<VulkanResourceFactory>, resource_factory, MANGO_NO_INIT_VAULE)
    define_private_member(std::vector<RecreateCallback>, resource_recreate_callbacks, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<std::unique_ptr<VulkanCommand>>, commands, MANGO_NO_INIT_VAULE)

    declare_component_cls_custom_construction(VulkanContext)
    };

    extern VulkanContext *vulkan_context;
}
