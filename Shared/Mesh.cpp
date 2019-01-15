/// Author: Ralph Ridley
/// Date: 15/01/19
#include "Mesh.h"

using namespace QZL;

Naive::TexturedBasicMesh Naive::basicToTextured(const BasicMesh& bmesh)
{
	Naive::TexturedBasicMesh tbm;
	tbm.vaoId = bmesh.vaoId;
	tbm.indexCount = bmesh.indexCount;
	tbm.tranform = bmesh.transform;
	tbm.texId = 0;
	return tbm;
}
