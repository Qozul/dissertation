/// Author: Ralph Ridley
/// Date: 15/01/19
#include "MeshLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace QZL;
using namespace QZL::Shared;

Naive::BasicMesh MeshLoader::loadNaiveMesh(const std::string& meshName)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	std::string warn;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshName.c_str());
	if (!warn.empty())
		std::cout << warn << std::endl;
	if (!err.empty())
		std::cout << err << std::endl;
	
	Naive::BasicMesh mesh;
	GLuint vbo, ibo, vao;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glGenVertexArrays(1, &vao);
	vertexBuffers_.push_back(vbo);
	indexBuffers_.push_back(ibo);
	vertexArrays_.push_back(vao);
	mesh.vaoId = vao;

	std::vector<size_t> indices;
	for (size_t i = 0; i < shapes.size(); i++) {
		mesh.indexCount = static_cast<GLsizei>(shapes[i].mesh.indices.size());

		// Using vertex positions as the indices buffer
		std::vector<tinyobj::index_t> sps = shapes[i].mesh.indices;
		std::vector<short> buffer(sps.size());

		std::vector<Vertex> verts(attrib.vertices.size() / 3);

		// Set the indices buffer and sort texcoords and normals
		for (size_t j = 0; j < sps.size(); j++) {
			tinyobj::index_t index = sps[j];
			buffer[j] = index.vertex_index;

			int vi3 = index.vertex_index * 3;
			int ti2 = index.texcoord_index * 2;
			int ni3 = index.normal_index * 3;

			// type double for highest precision, it will be cast down when placed in the vertex
			float u = 0.0, v = 0.0;
			float nx = 0.0, ny = 0.0, nz = 0.0;
			if (!attrib.texcoords.empty()) {
				u = attrib.texcoords[ti2];
				v = 1.0f - attrib.texcoords[ti2 + 1];
			}
			if (!attrib.normals.empty()) {
				nx = attrib.normals[ni3];
				ny = attrib.normals[ni3 + 1];
				nz = attrib.normals[ni3 + 2];
			}

			Vertex vertex;
			vertex.x = attrib.vertices[vi3];
			vertex.y = attrib.vertices[vi3 + 1];
			vertex.z = attrib.vertices[vi3 + 2];
			vertex.u = u;
			vertex.v = v;
			vertex.nx = nx;
			vertex.ny = ny;
			vertex.nz = nz;

			verts[index.vertex_index] = vertex;
		}

		glBindVertexArray(vao);

		// Put element data in the index buffer object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size() * sizeof(short), buffer.data(), GL_STATIC_DRAW);

		// VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // x, y, z
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(float))); // u, v
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(5 * sizeof(float))); // nx, ny, nz

		// Unbind the buffer, writing is finished
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	return mesh;
}
