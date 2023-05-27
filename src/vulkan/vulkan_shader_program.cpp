#include "vulkan_shader_program.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    void VulkanShaderProgram::bind_subpass(const char *subpass_name) {
        this->subpass_index = vulkan_context->get_render_pass().get_subpass_index_by_name(subpass_name);
        vulkan_context->get_render_pass().get_subpasses()[this->subpass_index].bind_shader_program(this);
    }

    void VulkanShaderProgram::set_topology(Topology topology) {
        this->topology = topology;
    }

    void VulkanShaderProgram::set_polygon_mode(PolygonMode polygon) {
        this->polygon = polygon;
    }
    
    void VulkanShaderProgram::set_front_face(FrontFace front) {
        this->front = front;
    }

    void VulkanShaderProgram::set_cull_mode(CullMode cull) {
        this->cull = cull;
    }

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
        _current_location = 0;
        _current_offset = 0;
    }

    void VulkanShaderProgram::attach_vertex_shader(Shader *shader, const char *entry) {
        this->vertex_shader = VulkanShaderInfo {
            .shader = (VulkanShader *)shader,
            .entry = entry,
        };
    }

    void VulkanShaderProgram::attach_fragment_shader(Shader *shader, const char *entry) {
        this->fragment_shader = VulkanShaderInfo {
            .shader = (VulkanShader *)shader,
            .entry = entry,
        };
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
        input_assembly_state.topology = topology2vk_primitive_topology(topology);

        // TODO: tessellation_state

        VkPipelineViewportStateCreateInfo viewport_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        VkViewport viewport {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(vulkan_context->get_extent().width),
            .height = static_cast<float>(vulkan_context->get_extent().height),
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
        rasterization_state.depthClampEnable = VK_FALSE;
        rasterization_state.depthBiasEnable = VK_FALSE;
        rasterization_state.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state.lineWidth = 1.0f;
        rasterization_state.polygonMode = polygon2vk_polygon_mode(polygon);
        rasterization_state.frontFace = front_face2vk_front_face(front);
        rasterization_state.cullMode = cull_mode2vk_cull_mode_flags(cull);

        VkPipelineMultisampleStateCreateInfo multisample_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisample_state.sampleShadingEnable = VK_FALSE;
        multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depth_stencil_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depth_stencil_state.stencilTestEnable = VK_FALSE;
        depth_stencil_state.depthTestEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blend_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        VkPipelineColorBlendAttachmentState color_blend_attachment {};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        color_blend_state.logicOpEnable = VK_FALSE;
        color_blend_state.attachmentCount = 1;
        color_blend_state.pAttachments = &color_blend_attachment;

        VkPipelineDynamicStateCreateInfo dynamic_state { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamic_state.pDynamicStates = dynamic_states.data();
        dynamic_state.dynamicStateCount = dynamic_states.size();

        VkPipelineLayoutCreateInfo layout_create_info { .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        layout_create_info.pSetLayouts = nullptr;
        layout_create_info.setLayoutCount = 0;
        layout_create_info.pPushConstantRanges = nullptr;
        layout_create_info.pushConstantRangeCount = 0;
        VK_CHECK(vkCreatePipelineLayout(vulkan_context->get_device().get_logical_device(), &layout_create_info, vulkan_context->get_allocator(), &layout))
        RHI_DEBUG("Create vulkan pipeline layout -> 0x{:x}", (AddrType)layout)

        VkGraphicsPipelineCreateInfo pipeline_create_info { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipeline_create_info.layout = layout;
        pipeline_create_info.renderPass = vulkan_context->get_render_pass().get_render_pass();
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
        VK_CHECK(vkCreateGraphicsPipelines(vulkan_context->get_device().get_logical_device(), VK_NULL_HANDLE, 1, &pipeline_create_info, vulkan_context->get_allocator(), &pipeline))
        RHI_DEBUG("Create vulkan pipeline -> 0x{:x}", (AddrType)pipeline)

        return Result::eSuccess;
    }

    Result VulkanShaderProgram::destroy() {
        component_destroy()

        RHI_DEBUG("Destroy vulkan pipeline -> 0x{:x}", (AddrType)pipeline)
        vkDestroyPipeline(vulkan_context->get_device().get_logical_device(), pipeline, vulkan_context->get_allocator());

        RHI_DEBUG("Destroy vulkan pipeline layout -> 0x{:x}", (AddrType)layout)
        vkDestroyPipelineLayout(vulkan_context->get_device().get_logical_device(), layout, vulkan_context->get_allocator());

        return Result::eSuccess;
    }
}