#version 450

vec2 poses[] = vec2[](
    vec2(1, -1),
    vec2(1, 1),
    vec2(-1, -1),
    vec2(-1, 1)
);

void main() {
    gl_Position = vec4(poses[gl_VertexIndex + gl_InstanceIndex], 0, 1);
}
