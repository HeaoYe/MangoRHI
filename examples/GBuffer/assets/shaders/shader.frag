#version 450

layout(location = 0) in vec3 frag_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec3 frag_color;
layout(location = 3) in vec2 frag_uv;

layout(location = 0) out vec4 out_pos;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_texture;

layout(binding = 0) uniform sampler2D tex_sampler;

void main() {
    out_pos = vec4(frag_pos, 1);
    out_normal = normalize(vec4(frag_normal, 0));
    out_texture = vec4(frag_color, 1) * texture(tex_sampler, frag_uv);
}
