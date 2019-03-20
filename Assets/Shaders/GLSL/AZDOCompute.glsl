#version 440

struct Transform {
	float raX, raY, raZ; 
	float pX, pY, pZ;
	float sX, sY, sZ;
	float rotationAngle;
};

layout(std430, binding = 1) buffer OUT0
{
    Transform[] oTransform;
};

uniform float uRotationAmount = 0.1;

layout(local_size_x=32) in;

void main(void)
{
	Transform old = oTransform[gl_GlobalInvocationID.x];
	old.rotationAngle += uRotationAmount;
	oTransform[gl_GlobalInvocationID.x] = old;
}
