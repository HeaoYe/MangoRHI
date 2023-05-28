#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

layout(binding = 0) uniform SRT {
    float scale;
    float rotate;
} srt[2];

vec2 pos[] = vec2[](
    vec2(1, 1),
    vec2(1, -1),
    vec2(-1, -1),
    vec2(-1, 1)
);

layout(location = 0) out vec3 frag_color;

void main() {
    vec3 offset = vec3(pos[gl_InstanceIndex], 0) ;
    mat3 rotate0 = mat3(
        cos(srt[0].rotate), sin(srt[0].rotate), 0,
        -sin(srt[0].rotate), cos(srt[0].rotate), 0,
        0, 0, 1
    );
    mat3 rotate1 = mat3(
        cos(srt[1].rotate), sin(srt[1].rotate), 0,
        -sin(srt[1].rotate), cos(srt[1].rotate), 0,
        0, 0, 1
    );
    gl_Position = vec4((rotate0 * in_pos * srt[0].scale + rotate1 * offset * srt[1].scale), 1);
    frag_color = in_color;
}
