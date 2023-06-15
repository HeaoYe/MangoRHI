#include "vulkan_shader_program.hpp"
#include "../vulkan_context.hpp"

namespace MangoRHI {
    void VulkanShaderProgram::add_vertex_attribute(VertexInputType type, u32 stride) {
        this->attributes.push_back(VkVertexInputAttributeDescription {
            .location = _current_location,
            .binding = static_cast<u32>(this->bindings.size()),
            .format = vertex_input_type2vk_format(type),
            .offset = _current_offset,
        });
        _current_location++;
        _current_offset += stride;
    }

    void VulkanShaderProgram::add_vertex_binding(VertexInputRate rate) {
        this->bindings.push_back(VkVertexInputBindingDescription {
            .binding = static_cast<u32>(this->bindings.size()),
            .stride = _current_offset,
            .inputRate = vertex_input_rate2vk_vertex_input_rate(rate),
        });
        _current_offset = 0;
    }

    void VulkanShaderProgram::attach_vertex_shader(const Shader &shader, const char *entry) {
        this->vertex_shader = VulkanShaderInfo {
            .shader = (VulkanShader *)&shader,
            .entry = entry,
        };
    }

    void VulkanShaderProgram::attach_fragment_shader(const Shader &shader, const char *entry) {
        this->fragment_shader = VulkanShaderInfo {
            .shader = (VulkanShader *)&shader,
            .entry = entry,
        };
    }

    std::weak_ptr<DescriptorSetLayout> VulkanShaderProgram::create_descriptor_set_layout(const char *layout_name) {
        auto &vk_layout = descriptor_set_layouts.emplace_back(new VulkanDescriptorSetLayout());
        vk_layout->set_name(layout_name);
        return vk_layout;
    }

    std::unique_ptr<DescriptorSet> VulkanShaderProgram::allocate_descriptor_set(const char *layout_name) {
        std::unique_ptr<VulkanDescriptorSet> descriptor_set = std::make_unique<VulkanDescriptorSet>();
        Bool found = MG_FALSE;
        std::for_each(descriptor_set_layouts.begin(), descriptor_set_layouts.end(), [layout_name, &descriptor_set, &found](auto &layout) {
            if (strcmp(layout_name, layout->get_name()) == 0) {
                if (layout->is_destroyed() == MG_TRUE) {
                    layout->create();
                }
                vulkan_context->get_descriptor_pool()->allocate(*layout, *descriptor_set);
                found = MG_TRUE;
                return;
            }
        });
        if (found == MG_FALSE) {
            RHI_ERROR("Cannot found descriptor layout {}", layout_name)
        }
        return std::move(descriptor_set);
    }

    void VulkanShaderProgram::free_descriptor_set(std::unique_ptr<DescriptorSet> descriptor_set) {
        vulkan_context->get_descriptor_pool()->free(dynamic_cast<VulkanDescriptorSet &>(*descriptor_set));
    }

    Result VulkanShaderProgram::create() {
        component_create()

        VkPipelineShaderStageCreateInfo shader_stages[2] = {
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertex_shader.shader->get_shader_module(),
                .pName = vertex_shader.entry,
            },
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragment_shader.shader->get_shader_module(),
                .pName = fragment_shader.entry,
            }
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertex_input_state.pVertexAttributeDescriptions = attributes.data();
        vertex_input_state.vertexAttributeDescriptionCount = attributes.size();
        vertex_input_state.pVertexBindingDescriptions = bindings.data();
        vertex_input_state.vertexBindingDescriptionCount = bindings.size();

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        input_assembly_state.primitiveRestartEnable = VK_FALSE;
        input_assembly_state.topology = topology;

        // TODO: tessellation_state

        VkPipelineViewportStateCreateInfo viewport_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        VkViewport viewport {
            .x = 0.0f,
            .y = static_cast<float>(vulkan_context->get_extent().height),
            .width = static_cast<float>(vulkan_context->get_extent().width),
            .height = -static_cast<float>(vulkan_context->get_extent().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        VkRect2D scissor {
            .offset = { .x = 0, .y = 0 },
            .extent = vulkan_context->get_extent(),
        };
        viewport_state.pViewports = &viewport;
        viewport_state.viewportCount = 1;
        viewport_state.pScissors = &scissor;
        viewport_state.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterization_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterization_state.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state.polygonMode = polygon_mode;
        rasterization_state.depthBiasEnable = VK_FALSE;
        rasterization_state.depthClampEnable = VK_FALSE;
        rasterization_state.frontFace = front_face;
        rasterization_state.cullMode = cull_mode;
        rasterization_state.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisample_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisample_state.rasterizationSamples = vulkan_context->get_multisample_count();
        multisample_state.sampleShadingEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depth_stencil_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depth_stencil_state.depthTestEnable = depth_test_enabled;
        depth_stencil_state.depthWriteEnable = depth_write_enabled;
        depth_stencil_state.depthCompareOp = depth_compare_op;
        depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_state.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blend_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        color_blend_state.logicOpEnable = VK_FALSE;
        color_blend_state.attachmentCount = vulkan_context->get_render_pass()->get_subpasses()[subpass_index]->get_color_blend_states().size();
        color_blend_state.pAttachments = vulkan_context->get_render_pass()->get_subpasses()[subpass_index]->get_color_blend_states().data();

        dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
        VkPipelineDynamicStateCreateInfo dynamic_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamic_state.pDynamicStates = dynamic_states.data();
        dynamic_state.dynamicStateCount = dynamic_states.size();

        STL_IMPL::vector<VkDescriptorSetLayout> layouts;
        std::for_each(descriptor_set_layouts.begin(), descriptor_set_layouts.end(), [&layouts](auto &layout) {
            if (layout->is_destroyed() == MG_TRUE) {
                layout->create();
            }
            layouts.push_back(layout->get_layout());
        });

        VkPipelineLayoutCreateInfo layout_create_info { .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        layout_create_info.pSetLayouts = layouts.data();
        layout_create_info.setLayoutCount = layouts.size();
        layout_create_info.pPushConstantRanges = nullptr;
        layout_create_info.pushConstantRangeCount = 0;
        VK_CHECK(vkCreatePipelineLayout(vulkan_context->get_device()->get_logical_device(), &layout_create_info, vulkan_context->get_allocator(), &layout))
        RHI_DEBUG("Create vulkan pipeline layout -> 0x{:x}", (AddrType)layout)

        VkGraphicsPipelineCreateInfo pipeline_create_info { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipeline_create_info.layout = layout;
        pipeline_create_info.renderPass = vulkan_context->get_render_pass()->get_render_pass();
        pipeline_create_info.subpass = subpass_index;
        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_create_info.basePipelineIndex = 0;
        pipeline_create_info.pStages = shader_stages;
        pipeline_create_info.stageCount = 2;
        pipeline_create_info.pVertexInputState = &vertex_input_state;
        pipeline_create_info.pInputAssemblyState = &input_assembly_state;
        // pipeline_create_info.pTessellationState = &tessellation_state;
        pipeline_create_info.pTessellationState = nullptr;
        pipeline_create_info.pViewportState = &viewport_state;
        pipeline_create_info.pRasterizationState = &rasterization_state;
        pipeline_create_info.pMultisampleState = &multisample_state;
        pipeline_create_info.pDepthStencilState = &depth_stencil_state;
        pipeline_create_info.pColorBlendState = &color_blend_state;
        pipeline_create_info.pDynamicState = &dynamic_state;
        VK_CHECK(vkCreateGraphicsPipelines(vulkan_context->get_device()->get_logical_device(), VK_NULL_HANDLE, 1, &pipeline_create_info, vulkan_context->get_allocator(), &pipeline))
        RHI_DEBUG("Create vulkan pipeline -> 0x{:x}", (AddrType)pipeline)

        return Result::eSuccess;
    }

    Result VulkanShaderProgram::destroy() {
        component_destroy()

        VK_CHECK(vkDeviceWaitIdle(vulkan_context->get_device()->get_logical_device()))

        RHI_DEBUG("Destroy vulkan pipeline -> 0x{:x}", (AddrType)pipeline)
        vkDestroyPipeline(vulkan_context->get_device()->get_logical_device(), pipeline, vulkan_context->get_allocator());

        RHI_DEBUG("Destroy vulkan pipeline layout -> 0x{:x}", (AddrType)layout)
        vkDestroyPipelineLayout(vulkan_context->get_device()->get_logical_device(), layout, vulkan_context->get_allocator());

        descriptor_set_layouts.clear();

        component_destroy_end()
    }
}
