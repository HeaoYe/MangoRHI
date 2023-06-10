#include "vulkan_resource_manager.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    RenderTarget &VulkanResourceManager::create_render_target(const char *name, RenderTargetUsage usage) {
        auto &render_target = render_targets.emplace_back(new VulkanRenderTarget());
        render_target->set_name(name);
        render_target->set_usage(usage);
        vulkan_context->get_render_pass()->attach_render_target(*render_target);
        if (vulkan_context->is_destroyed() == MG_FALSE) {
            render_target->create();
        }
        return *render_target;
    }

    Shader &VulkanResourceManager::create_shader(const char *filename) {
        auto &shader = shaders.emplace_back(new VulkanShader());
        shader->set_filename(filename);
        if (vulkan_context->is_destroyed() == MG_FALSE) {
            shader->create();
        }
        return *shader;
    }

    VertexBuffer &VulkanResourceManager::create_vertex_buffer(u32 vertex_size, u32 count) {
        auto &vertex_buffer = vertex_buffers.emplace_back(new VulkanVertexBuffer());
        vertex_buffer->set_vertex_size(vertex_size);
        vertex_buffer->set_count(count);
        if (vulkan_context->is_destroyed() == MG_FALSE) {
            vertex_buffer->create();
        }
        return *vertex_buffer;
    }

    IndexBuffer &VulkanResourceManager::create_index_buffer(u32 count) {
        auto &index_buffer = index_buffers.emplace_back(new VulkanIndexBuffer);
        index_buffer->set_count(count);
        if (vulkan_context->is_destroyed() == MG_FALSE) {
            index_buffer->create();
        }
        return *index_buffer;
    }

    Texture &VulkanResourceManager::create_texture(const char *filename, u32 mipmap_levels) {
        auto &texture = textures.emplace_back(new VulkanTexture());
        texture->set_filename(filename);
        texture->set_is_empty(MG_FALSE);
        texture->set_mipmap_levels(mipmap_levels);
        if (vulkan_context->is_destroyed() == MG_FALSE) {
            texture->create();
        }
        return *texture;
    }
    Texture &VulkanResourceManager::create_empty_texture(u32 mipmap_levels) {
        auto &texture = textures.emplace_back(new VulkanTexture());
        texture->set_filename(nullptr);
        texture->set_is_empty(MG_TRUE);
        texture->set_mipmap_levels(mipmap_levels);
        if (vulkan_context->is_destroyed() == MG_FALSE) {
            texture->create();
        }
        return *texture;
    }

    Sampler &VulkanResourceManager::create_sampler() {
        auto &sampler = samplers.emplace_back(new VulkanSampler());
        if (vulkan_context->is_destroyed() == MG_FALSE) {
            sampler->create();
        }
        return *sampler;
    }

    ShaderProgram &VulkanResourceManager::create_shader_program(const char *subpass_name) {
        auto &shader_program = shader_programs.emplace_back(new VulkanShaderProgram());
        shader_program->set_subpass_index(vulkan_context->get_render_pass()->get_subpass_index_by_name(subpass_name));
        if (vulkan_context->is_destroyed() == MG_FALSE) {
            shader_program->create();
        }
        return *shader_program;
    }

    Result VulkanResourceManager::create() {
        component_create()

        for (auto &render_target : render_targets) {
            render_target->create();
        }
        for (auto &shader : shaders) {
            shader->create();
        }
        for (auto &texture : textures) {
            texture->create();
        }
        for (auto &sampler : samplers) {
            sampler->create();
        }
        for (auto &vertex_buffer : vertex_buffers) {
            vertex_buffer->create();
        }
        for (auto &index_buffer : index_buffers) {
            index_buffer->create();
        }

        return Result::eSuccess;
    }

    Result VulkanResourceManager::destroy() {
        component_destroy()

        for (auto &index_buffer : index_buffers) {
            index_buffer->destroy();
        }
        index_buffers.clear();
        for (auto &vertex_buffer : vertex_buffers) {
            vertex_buffer->destroy();
        }
        vertex_buffers.clear();
        for (auto &sampler : samplers) {
            sampler->destroy();
        }
        samplers.clear();
        for (auto &texture : textures) {
            texture->destroy();
        }
        textures.clear();
        for (auto &shader : shaders) {
            shader->destroy();
        }
        shaders.clear();
        for (auto &render_target : render_targets) {
            render_target->destroy();
        }
        render_targets.clear();

        return Result::eSuccess;
    }

    Result VulkanResourceManager::post_create() {
        if (is_destroyed() == MG_TRUE) {
            return Result::eFailed;
        }
        for (auto &shader_program : shader_programs) {
            shader_program->create();
        }
        return Result::eSuccess;
    }

    Result VulkanResourceManager::pre_destroy() {
        if (is_destroyed() == MG_TRUE) {
            return Result::eFailed;
        }
        for (auto &shader_program : shader_programs) {
            shader_program->destroy();
        }
        shader_programs.clear();
        return Result::eSuccess;
    }

    void VulkanResourceManager::recreate_render_targets() {
        for (auto &render_target : render_targets) {
            render_target->recreate();
        }
    }
}
