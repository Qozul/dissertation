#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTextureCoord;
layout(location = 2) in vec3 iNormal;

layout(location = 0) out vec4 oColor;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = vec4(iPosition.xy, 0.0, 1.0);
	oColor = vec4(iNormal, 1.0);
}
