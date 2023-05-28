#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

layout(binding = 0) uniform SRT {
    float scale;
    float rotate;
} srt;

vec2 pos[] = vec2[](
    vec2(1, 1),
    vec2(1, -1),
    vec2(-1, -1),
    vec2(-1, 1)
);

layout(location = 0) out vec3 frag_color;

void main() {
    vec3 offset = vec3(pos[gl_InstanceIndex], 0);
    mat3 rotate = mat3(
        cos(srt.rotate), sin(srt.rotate), 0,
        -sin(srt.rotate), cos(srt.rotate), 0,
        0, 0, 1
    );
    gl_Position = vec4(transpose(rotate) * (rotate * rotate * in_pos + offset) * srt.scale / 2, 1);
    frag_color = in_color * (gl_VertexIndex + 1) / 3;
}
