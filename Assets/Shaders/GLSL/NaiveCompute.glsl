#version 440

struct Transform {
	vec3 position;
	vec3 rotationAxis;
	vec3 scale;
	float rotationAngle;
};

layout(std430, binding = 0) writeonly buffer OUT0
{
    Transform oTransform;
};

uniform float uCurrentAngle;
uniform float uRotationAmount = 0.1;

layout(local_size_x=1) in;

void main(void)
{
	oTransform.rotationAngle = uCurrentAngle + uRotationAmount;
}
