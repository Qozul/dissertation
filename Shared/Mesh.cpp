/// Author: Ralph Ridley
/// Date: 15/01/19
#include "Mesh.h"

using namespace QZL;

Naive::TexturedBasicMesh* Naive::basicToTextured(const BasicMesh& bmesh)
{
	Naive::TexturedBasicMesh* tbm = new Naive::TexturedBasicMesh();
	tbm->vaoId = bmesh.vaoId;
	tbm->indexCount = bmesh.indexCount;
	tbm->transform = bmesh.transform;
	tbm->texture = nullptr;
	return tbm;
}
