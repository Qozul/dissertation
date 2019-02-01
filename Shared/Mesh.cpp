/// Author: Ralph Ridley
/// Date: 15/01/19
#include "Mesh.h"

using namespace QZL;

Naive::TexturedBasicMesh* Naive::basicToTextured(BasicMesh* bmesh, Texture* texture)
{
	Naive::TexturedBasicMesh* tbm = new Naive::TexturedBasicMesh();
	tbm->vaoId = bmesh->vaoId;
	tbm->indexCount = bmesh->indexCount;
	tbm->transform = bmesh->transform;
	tbm->texture = texture;

	SAFE_DELETE(bmesh);
	return tbm;
}


template<typename InstType>
InstType* AZDO::makeMeshInstance(const AZDO::BasicMesh& mesh)
{
	static_assert(std::is_same<InstType, AZDO::MeshInstance>::value || 
		std::is_same<InstType, AZDO::TexturedMeshInstance>::value, 
		"Type not allowed");

	InstType* inst = new InstType();
	inst->meshId = mesh.id;
	return inst;
}
