#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

layout(binding = 0) uniform SRT {
    float scale;
} srt;

layout(location = 0) out vec3 frag_color;

void main() {
    gl_Position = vec4(in_pos * srt.scale, 1);
    frag_color = in_color;
}
