#version 440 core

uniform mat4 uMVP;

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTexUV;
layout(location = 2) in vec3 iNormal;

out Vertex
{
	vec2 texUV;
} OUT;

void main(void)
{
	gl_Position	= uMVP * vec4(iPosition, 1.0);
	OUT.texUV = iTexUV;
}
