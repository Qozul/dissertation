#version 440 core

uniform sampler2D tDiffuse;

in Vertex
{
	vec2 texUV;
} IN;

out vec4 fragColor;

void main(void)
{	
	fragColor = texture(tDiffuse, IN.texUV);
}
