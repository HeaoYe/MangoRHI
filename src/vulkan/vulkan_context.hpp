#pragma once

#include "MangoRHI/context.hpp"
#include "vulkan_commons.hpp"
#include "vulkan_device.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_renderpass.hpp"
#include "resource/vulkan_shader.hpp"
#include "vulkan_shader_program.hpp"
#include "vulkan_framebuffer.hpp"
#include "vulkan_synchronization.hpp"
#include "vulkan_command_pool.hpp"
#include "resource/vulkan_buffer.hpp"
#include "vulkan_descriptor_pool.hpp"

namespace MangoRHI {
    class VulkanContext final : public Context {
    public:
        void set_api_info(const void *info) override;
        void set_device_name(const char *name) override;
        void set_swapchain_image_count(u32 count) override;
        void set_max_in_flight_frame_count(u32 count) override;
        void set_clear_color(ColorClearValue clear_color) override;
        Shader *create_shader(const char *filename) override;
        VertexBuffer *create_vertex_buffer() override;
        IndexBuffer *create_index_buffer() override;

        void resize(u32 width, u32 height) override;
        const u32 get_width() const override { return extent.width; }
        const u32 get_height() const override { return extent.height; }
        RenderPass &get_render_pass_reference() override { return render_pass; }
        Command &get_current_command_reference() override { return *commands[current_in_flight_frame_index]; };
        
        Result create() override;
        Result destroy() override;

        Result begin_frame() override;
        Result end_frame() override;

        VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect) const;
        u32 find_memory_index(u32 type_filter, VkMemoryPropertyFlags flags) const;

    define_pointer(VkAllocationCallbacks, allocator, VK_NULL_HANDLE)
    define_member(VkInstance, instance, VK_NULL_HANDLE)
    define_member(VkSurfaceKHR, surface, VK_NULL_HANDLE)
    define_member(VulkanDevice, device, MANGO_NO_INIT_VAULE)
    define_member(VulkanSwapchain, swapchain, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VulkanRenderPass, render_pass, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VulkanFrameBuffer, framebuffer, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VulkanVertexBuffer *>, vertex_buffers, MANGO_NO_INIT_VAULE)
    define_member(STL_IMPL::vector<VulkanIndexBuffer *>, index_buffers, MANGO_NO_INIT_VAULE)
    define_member(VulkanDescriptorPool, descriptor_pool, MANGO_NO_INIT_VAULE)
    define_member(VulkanSynchronization, synchronization, MANGO_NO_INIT_VAULE)
    define_member(VulkanCommandPool, command_pool, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(VkExtent2D, extent, MANGO_NO_INIT_VAULE)
    define_member(u32, max_in_flight_frame_count, 2)
    define_private_readonly_pointer(VulkanContextInfo, info, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanShader *>, shaders, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<VulkanCommand *>, commands, MANGO_NO_INIT_VAULE)
    define_member(u32, current_in_flight_frame_index, 0)

    no_copy_and_move_construction(VulkanContext)
    };

    extern VulkanContext *vulkan_context;
}
