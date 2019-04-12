#version 450
#extension GL_ARB_separate_shader_objects : enable

struct ElementData {
	mat4 model;
    mat4 mvp;
};

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTextureCoord;
layout(location = 2) in vec3 iNormal;

layout(location = 0) out vec2 out_textureCoord;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(binding = 0) uniform UniformBufferObject {
    ElementData[10] uElementData;
} ubo;

void main() {
	gl_Position = ubo.uElementData[gl_InstanceIndex].mvp * vec4(iPosition, 1.0);
	out_textureCoord = iTextureCoord;
}
