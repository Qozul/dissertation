#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 in_textureCoord;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
	out_color = texture(texSampler, in_textureCoord);
}
