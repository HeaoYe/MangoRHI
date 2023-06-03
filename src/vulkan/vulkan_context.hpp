#pragma once

#include "MangoRHI/context.hpp"
#include "vulkan_commons.hpp"
#include "vulkan_device.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_renderpass.hpp"
#include "vulkan_shader_program.hpp"
#include "vulkan_framebuffer.hpp"
#include "vulkan_synchronization.hpp"
#include "vulkan_command_pool.hpp"
#include "vulkan_descriptor_pool.hpp"
#include "resource/vulkan_shader.hpp"
#include "resource/vulkan_buffer.hpp"
#include "resource/vulkan_sampler.hpp"
#include "resource/vulkan_texture.hpp"

namespace MangoRHI {
    struct VulkanContextCreateInfo {
        
    };
    class VulkanContext final : public Context {
    public:
        void set_api_info(const void *info) override;
        void set_device_name(const char *name) override;
        void set_swapchain_image_count(u32 count) override;
        void set_max_in_flight_frame_count(u32 count) override;
        void set_clear_color(ColorClearValue clear_color) override;
        RenderTarget *create_render_target() override;
        Shader *create_shader(const char *filename) override;
        VertexBuffer *create_vertex_buffer() override;
        IndexBuffer *create_index_buffer() override;
        Texture *create_texture() override;

        void resize(u32 width, u32 height) override;
        const u32 get_width() const override { return extent.width; }
        const u32 get_height() const override { return extent.height; }
        RenderPass &get_render_pass_reference() override { return render_pass; }
        Command &get_current_command_reference() override { return *commands[current_in_flight_frame_index]; };
        
        Result create() override;
        Result destroy() override;

        Result begin_frame() override;
        Result end_frame() override;

        VkFormat find_supported_format(const STL_IMPL::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
        VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect, uint32_t mipmap_levels) const;
        u32 find_memory_index(u32 type_filter, VkMemoryPropertyFlags flags) const;
        void transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mipmap_levels) const;

    private:
        void recreate_resources();
    
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkFormat, depth_format, VK_FORMAT_UNDEFINED)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkSampleCountFlagBits, max_msaa_samples, MANGO_NO_INIT_VAULE)

    define_pointer(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkAllocationCallbacks, allocator, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkInstance, instance, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkSurfaceKHR, surface, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanDevice, device, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanSwapchain, swapchain, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanRenderPass, render_pass, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanFrameBuffer, framebuffer, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanDescriptorPool, descriptor_pool, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanSynchronization, synchronization, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanCommandPool, command_pool, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, VkExtent2D, extent, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, u32, max_in_flight_frame_count, 2)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, u32, current_in_flight_frame_index, 0)
    define_private_readonly_pointer(VulkanContextInfo, info, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanRenderTarget *>, render_targets, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanShader *>, shaders, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanVertexBuffer *>, vertex_buffers, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanIndexBuffer *>, index_buffers, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanTexture *>, textures, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanCommand *>, commands, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanContext)
    };

    extern VulkanContext *vulkan_context;
}
