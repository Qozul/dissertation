/// Author: Ralph Ridley
/// Date: 15/01/19
#include "MeshLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace QZL;
using namespace QZL::Shared;

const std::string MeshLoader::kPath = "../Assets/Meshes/";
const std::string MeshLoader::kExt = ".obj";

Naive::BasicMesh* MeshLoader::loadNaiveMesh(const std::string& meshName)
{
	Naive::BasicMesh* mesh = new Naive::BasicMesh();

	auto meshIterator = naiveMeshes_.find(meshName);
	if (meshIterator != naiveMeshes_.end()) {
		mesh->vaoId = meshIterator->second.first;
		mesh->indexCount = meshIterator->second.second;
		return mesh;
	}

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	std::string warn;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (kPath + meshName + kExt).c_str());
	if (!warn.empty())
		std::cout << warn << std::endl;
	if (!err.empty())
		std::cout << err << std::endl;
	
	GLuint vbo, ibo, vao;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glGenVertexArrays(1, &vao);
	vertexBuffers_.push_back(vbo);
	indexBuffers_.push_back(ibo);
	vertexArrays_.push_back(vao);
	mesh->vaoId = vao;

	std::vector<GLushort> indices;
	std::vector<Vertex> verts;
	// TODO: remove duplicate vertices
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};
			vertex.x = attrib.vertices[3 * index.vertex_index + 0];
			vertex.y = attrib.vertices[3 * index.vertex_index + 1];
			vertex.z = attrib.vertices[3 * index.vertex_index + 2];
			vertex.u = attrib.texcoords[2 * index.texcoord_index + 0];
			vertex.v = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];
			vertex.nx = attrib.normals[3 * index.normal_index + 0];
			vertex.ny = attrib.normals[3 * index.normal_index + 1];
			vertex.nz = attrib.normals[3 * index.normal_index + 2];

			verts.push_back(vertex);
			indices.push_back(indices.size());
		}
	}
	mesh->indexCount = indices.size();
	glBindVertexArray(vao);

	// Put element data in the index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // x, y, z
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat))); // u, v
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(5 * sizeof(GLfloat))); // nx, ny, nz

	// Unbind the buffer, writing is finished
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	naiveMeshes_[meshName].first = mesh->vaoId;
	naiveMeshes_[meshName].second = mesh->indexCount;

	DEBUG_OUT("Loaded naive BasicMesh " << kPath << meshName << kExt);
	return mesh;
}
