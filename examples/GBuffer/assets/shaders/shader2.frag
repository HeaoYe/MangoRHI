#version 450

layout (input_attachment_index = 0, binding = 0) uniform subpassInput sampler_pos;
layout (input_attachment_index = 1, binding = 1) uniform subpassInput sampler_normal;
layout (input_attachment_index = 2, binding = 2) uniform subpassInput sampler_texture;

layout (location = 0) out vec4 out_color;

vec3 light = vec3(0, 1, 0);

void main() {
	vec4 pos = subpassLoad(sampler_pos);
	vec4 normal = subpassLoad(sampler_normal);
	float n = dot(light, normal.xyz);
	vec4 texture = n * subpassLoad(sampler_texture);
    out_color = texture;
}
