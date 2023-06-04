#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;

layout(location = 0) out vec3 frag_pos;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out vec3 frag_color;
layout(location = 3) out vec2 frag_uv;

void main() {
    gl_Position = vec4(in_pos, 1);
    frag_pos = in_pos;
    frag_normal = in_normal;
    frag_color = in_color;
    frag_uv = in_pos.xy + 0.5;
}
