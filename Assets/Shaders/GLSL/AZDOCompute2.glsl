#version 440

struct Transform {
	vec3 position;
	vec3 rotationAxis;
	vec3 scale;
	float rotationAngle;
};

struct InstanceData {
	mat4 model;
	mat4 mvp;
};

layout(std430, binding = 0) writeonly buffer OUT
{
	InstandData[] instanceData;
};

layout(std430, binding = 1) readonly buffer IN
{
    Transform[] transforms;
};

uniform float uRotationAmount = 0.1;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

layout(local_size_x=32) in;

// http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/
mat4 makeModelMatrix(Transform transform)
{
	float c = cos(transform.rotationAngle);
	float s = sin(transform.rotationAngle);
	float t = 1.0 - c;
	vec3 axis = normalize(transform.rotationAxis);
	
	float txx = t*axis.x*axis.x;
	float txy = t*axis.x*axis.y;
	float txz = t*axis.x*axis.z;
	float tyy = t*axis.y*axis.y;
	float tyz = t*axis.y*axis.z;
	float tzz = t*axis.z*axis.z;
	
	float sx = s*axis.x;
	float sy = s*axis.y;
	float sz = s*axis.z;
	
	mat4 scale = mat4(
					vec4(transform.scale.x, 0.0, 0.0, 0.0),
					vec4(0.0, transform.scale.y, 0.0, 0.0),
					vec4(0.0, 0.0, transform.scale.z, 0.0),
					vec4(0.0, 0.0, 0.0,               1.0));

	mat4 rotation = mat4(
						vec4(txx + c,  txy + sz, txz - sy, 0.0),
						vec4(txy - sz, tyy + c,  tyz + sz, 0.0),
						vec4(txz + sy, tyz - sx, tzz + c,  0.0),
						vec4(0.0,      0.0,      0.0,      1.0));
						
	mat4 modelMat = rotation * scale;

	// Translation directly
	modelMat[3][0] = transform.position.x;
	modelMat[3][1] = transform.position.y;
	modelMat[3][2] = transform.position.z;
	
	return modelMat;
}

void main(void)
{
	Transform trans = transforms[gl_GlobalInvocationID.x];
	trans.rotationAngle += uRotationAmount;
	mat4 model = makeModelMatrix(trans);
	instanceData[gl_GlobalInvocationID.x].model = model;
	instanceData[gl_GlobalInvocationID.x].mvp = uProjMatrix * uViewMatrix * model;
	
}
