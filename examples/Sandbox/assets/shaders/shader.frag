#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in float tex_index;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D tex_sampler[4];

void main() {
    vec4 tex_color = texture(tex_sampler[int(tex_index)], frag_uv);
    out_color = vec4(frag_color * (1 - tex_color.a) + tex_color.rgb * tex_color.a , 1);
}
