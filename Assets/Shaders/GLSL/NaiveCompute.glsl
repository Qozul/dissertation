#version 440

struct Transform {
	vec3 position;
	vec3 rotationAxis;
	vec3 scale;
	float rotationAngle;
};

layout(std430, binding = 0) buffer OUT0
{
    //Transform oTransform;
	float oTransform;
};

//uniform float uCurrentAngle = 0.0;
uniform float uRotationAmount = 0.1;

layout(local_size_x=1) in;

void main(void)
{
	oTransform = oTransform + uRotationAmount;
}
