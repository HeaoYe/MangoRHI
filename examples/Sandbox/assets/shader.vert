#version 440

layout(location = 0) out vec3 frag_color;

vec2 positions[6] = vec2[](
    vec2(1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0)
);

void main() {
    frag_color = vec3(0.3, (positions[gl_InstanceIndex * 2] + 1) / 2);
    gl_Position = vec4(positions[gl_InstanceIndex * 3 + gl_VertexIndex], 0, 0);
}
