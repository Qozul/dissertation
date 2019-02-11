#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTextureCoord;
layout(location = 2) in vec3 iNormal;

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
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(iPosition, 1.0);
	out_color = vec4(iNormal, 1.0);
}
