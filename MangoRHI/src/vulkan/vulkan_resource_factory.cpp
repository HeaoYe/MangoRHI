#include "vulkan_resource_factory.hpp"
#include "vulkan_context.hpp"

namespace MangoRHI {
    std::unique_ptr<Shader> VulkanResourceFactory::create_shader(const char *filename) {
        auto shader = std::unique_ptr<Shader>(new VulkanShader());
        shader->set_filename(filename);
        shader->create();
        return std::move(shader);
    }

    std::unique_ptr<VertexBuffer> VulkanResourceFactory::create_vertex_buffer(u32 vertex_size, u32 count) {
        auto vertex_buffer = std::unique_ptr<VertexBuffer>(new VulkanVertexBuffer());
        vertex_buffer->set_vertex_size(vertex_size);
        vertex_buffer->set_count(count);
        vertex_buffer->create();
        return std::move(vertex_buffer);
    }

    std::unique_ptr<IndexBuffer> VulkanResourceFactory::create_index_buffer(u32 count) {
        auto index_buffer = std::unique_ptr<IndexBuffer>(new VulkanIndexBuffer());
        index_buffer->set_count(count);
        index_buffer->create();
        return std::move(index_buffer);
    }

    std::unique_ptr<Texture> VulkanResourceFactory::create_texture(const char *filename, u32 mipmap_levels, Bool is_create) {
        auto texture = std::unique_ptr<Texture>(new VulkanTexture());
        texture->set_filename(filename);
        texture->set_is_empty(MG_FALSE);
        texture->set_mipmap_levels(mipmap_levels);
        if (is_create == MG_TRUE) {
            texture->create();
        }
        return std::move(texture);
    }

    std::unique_ptr<Texture> VulkanResourceFactory::create_empty_texture(u32 mipmap_levels) {
        auto texture = std::unique_ptr<Texture>(new VulkanTexture());
        texture->set_filename(nullptr);
        texture->set_is_empty(MG_TRUE);
        texture->set_mipmap_levels(mipmap_levels);
        texture->create();
        return std::move(texture);
    }

    std::unique_ptr<Sampler> VulkanResourceFactory::create_sampler(Bool is_create) {
        auto sampler = std::unique_ptr<Sampler>(new VulkanSampler());
        if (is_create == MG_TRUE) {
            sampler->create();
        }
        return std::move(sampler);
    }

    std::unique_ptr<ShaderProgram> VulkanResourceFactory::create_shader_program(const char *subpass_name) {
        auto shader_program = std::unique_ptr<ShaderProgram>(new VulkanShaderProgram());
        ((VulkanShaderProgram &)*shader_program).set_subpass_index(vulkan_context->get_render_pass()->get_subpass_index_by_name(subpass_name));
        return std::move(shader_program);
    }

    Result VulkanResourceFactory::create() {
        component_create()

        return Result::eSuccess;
    }

    Result VulkanResourceFactory::destroy() {
        component_destroy()

        component_destroy_end()
    }
}
