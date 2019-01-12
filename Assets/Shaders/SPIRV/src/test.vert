#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec4 out_color;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
	out_color = vec4(in_color, 1.0);
}
