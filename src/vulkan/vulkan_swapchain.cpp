#include "vulkan_swapchain.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    Result VulkanSwapchain::create() {
        component_create()

        SwapchainDetails details {};
        vulkan_context->get_device().query_swapchain_details(details);
        vulkan_context->get_extent().width = STL_IMPL::clamp<u32>(details.capabilities.currentExtent.width, details.capabilities.minImageExtent.width, details.capabilities.maxImageExtent.width);
        vulkan_context->get_extent().height = STL_IMPL::clamp<u32>(details.capabilities.currentExtent.height, details.capabilities.minImageExtent.height, details.capabilities.maxImageExtent.height);

        Bool found = MG_FALSE;
        format = details.formats[0];
        for (const auto& a_format : details.formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                RHI_DEBUG("\tFound best surface format")
                format = a_format;
                found = MG_TRUE;
                break;
            }
        }
        if (found == MG_FALSE) {
            RHI_DEBUG("\tBest surface format not found")
        }

        found = MG_FALSE;
        present_mode = details.present_modes[0];
        for (const auto& a_present_mode : details.present_modes) {
            if (a_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                RHI_DEBUG("\tFound best surface present mode")
                present_mode = a_present_mode;
                found = MG_TRUE;
                break;
            }
        }
        if (found == MG_FALSE) {
            RHI_DEBUG("\tBest surface present mode not found")
        }

        if (image_count < details.capabilities.minImageCount) {
            RHI_WARN("image_count({0}) is less then minImageCount({1}), auto clamp to minImageCount{1}", image_count, details.capabilities.minImageCount)
            image_count = details.capabilities.minImageCount;
        } else if (details.capabilities.maxImageCount != 0 && image_count > details.capabilities.maxImageCount) {
            RHI_WARN("image_count({0}) is more then maxImageCount({1}), auto clamp to maxImageCount{1}", image_count, details.capabilities.maxImageCount)
            image_count = details.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapchain_create_info { .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchain_create_info.oldSwapchain = swapchain;
        swapchain_create_info.clipped = VK_TRUE;
        swapchain_create_info.surface = vulkan_context->get_surface();
        swapchain_create_info.preTransform = details.capabilities.currentTransform;
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_create_info.presentMode = present_mode;
        swapchain_create_info.imageFormat = format.format;
        swapchain_create_info.imageColorSpace = format.colorSpace;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.imageExtent = vulkan_context->get_extent();
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_create_info.minImageCount = image_count;
        if (vulkan_context->get_device().get_graphics_family_index() == vulkan_context->get_device().get_present_family_index()) {
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        } else {
            u32 queue_family_indices[] = { vulkan_context->get_device().get_graphics_family_index(), vulkan_context->get_device().get_present_family_index() };
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
            swapchain_create_info.queueFamilyIndexCount = 2;
        }
        VK_CHECK(vkCreateSwapchainKHR(vulkan_context->get_device().get_logical_device(), &swapchain_create_info, vulkan_context->get_allocator(), &swapchain))
        RHI_DEBUG("Create vulkan swapchain -> 0x{:x}", (AddrType)swapchain)

        u32 count;
        VK_CHECK(vkGetSwapchainImagesKHR(vulkan_context->get_device().get_logical_device(), swapchain, &count, nullptr))
        images.resize(count);
        image_views.resize(count);
        VK_CHECK(vkGetSwapchainImagesKHR(vulkan_context->get_device().get_logical_device(), swapchain, &count, images.data()))
        if (count != image_count) {
            RHI_WARN("Expecting image_count is {} but actually image_count is {}", image_count, count)
            image_count = count;
        }
        render_target.set_name(MANGORHI_SURFACE_RENDER_TARGET_NAME);
        render_target.set_usage(RenderTargetUsage::eColor);
        for (count = 0; count < image_count; count++) {
            image_views[count] = vulkan_context->create_image_view(images[count], format.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
            render_target.add_render_target_data(images[count], image_views[count]);
            RHI_DEBUG("Create vulkan swapchain image view({}) -> 0x{:x}", count, (AddrType)image_views[count])
        }
        render_target.create();

        return Result::eSuccess;
    };

    Result VulkanSwapchain::destroy() {
        component_destroy()

        render_target.destroy();

        for (u32 i = 0; i < image_count; i++) {
            RHI_DEBUG("Destroy vulkan swapchain image view({}) -> 0x{:x}", i, (AddrType)image_views[i])
            vkDestroyImageView(vulkan_context->get_device().get_logical_device(), image_views[i], vulkan_context->get_allocator());
        }

        RHI_DEBUG("Destroy vulkan swapchain -> 0x{:x}", (AddrType)swapchain)
        vkDestroySwapchainKHR(vulkan_context->get_device().get_logical_device(), swapchain, vulkan_context->get_allocator());
        swapchain = VK_NULL_HANDLE;

        return Result::eSuccess;
    };

    Result VulkanSwapchain::acquire_next_frame() {
        VK_CHECK(vkWaitForFences(vulkan_context->get_device().get_logical_device(), 1, &vulkan_context->get_synchronization().get_current_fence(), VK_TRUE, UINT64_MAX))

        auto res = vkAcquireNextImageKHR(vulkan_context->get_device().get_logical_device(), swapchain, UINT64_MAX, vulkan_context->get_synchronization().get_current_image_available_semaphore(), VK_NULL_HANDLE, &image_index);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
            return Result::eNeedToRecreate;
        }

        VK_CHECK(vkResetFences(vulkan_context->get_device().get_logical_device(), 1, &vulkan_context->get_synchronization().get_current_fence()))

        return Result::eSuccess;
    };

    Result VulkanSwapchain::present() {
        VkPresentInfoKHR present_info { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        present_info.pSwapchains = &swapchain;
        present_info.swapchainCount = 1;
        present_info.pWaitSemaphores = &vulkan_context->get_synchronization().get_current_render_finished_semaphore();
        present_info.waitSemaphoreCount = 1;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;
        auto res = vkQueuePresentKHR(vulkan_context->get_device().get_present_queue(), &present_info);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
            return Result::eNeedToRecreate;
        }

        return Result::eSuccess;
    };
}
