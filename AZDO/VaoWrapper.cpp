/// Author: Ralph Ridley
/// Date: 02/02/19
#include "VaoWrapper.h"
#include "../Shared/Mesh.h"
#include "../Shared/Vertex.h"

using namespace QZL;
using namespace QZL::AZDO;

VaoWrapper::VaoWrapper()
	: isCommitted_(false)
{
	glGenBuffers(1, &vboId_);
	glGenBuffers(1, &iboId_);
	glGenVertexArrays(1, &vaoId_);
}

VaoWrapper::~VaoWrapper()
{
	for (auto& it : meshes_) {
		SAFE_DELETE(it.second);
	}
	glDeleteBuffers(1, &vboId_);
	glDeleteBuffers(1, &iboId_);
	glDeleteVertexArrays(1, &vaoId_);
}

void VaoWrapper::commit()
{
	ENSURES(!isCommitted_)

	glBindVertexArray(vaoId_);

	// Put element data in the index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(Index), indices_.data(), GL_STATIC_DRAW);

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboId_);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Shared::Vertex), vertices_.data(), GL_STATIC_DRAW);

	// Interleave
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Shared::Vertex), (GLvoid*)0); // x, y, z
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Shared::Vertex), (GLvoid*)(3 * sizeof(GLfloat))); // u, v
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Shared::Vertex), (GLvoid*)(5 * sizeof(GLfloat))); // nx, ny, nz

	// Unbind the buffer, writing is finished
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// No longer need the data in this vao, so clear it
	indices_.clear();
	vertices_.clear();

	isCommitted_ = true;
}

size_t VaoWrapper::addVertices(Shared::Vertex* data, const size_t size)
{
	ENSURES(!isCommitted_)
	const size_t prevSize = vertices_.size();

	// Ensure buffer has room for new data
	vertices_.resize(prevSize + size);

	// Simply copy the data across directly
	std::copy_n(data, size, vertices_.begin() + prevSize);

	// First index of the new data is equal to the previous size (1 past the old data end)
	return prevSize;
}

size_t VaoWrapper::addIndices(Index * data, const size_t size)
{
	ENSURES(!isCommitted_);
	const size_t prevSize = indices_.size();
	indices_.resize(prevSize + size);
	std::copy_n(data, size, indices_.begin() + prevSize);
	return prevSize;
}

void VaoWrapper::emplaceMesh(std::string name, GLuint indexCount, GLuint indexOffset, GLuint vertexOffset)
{
	meshes_[name] = new BasicMesh();
	meshes_[name]->indexCount = indexCount;
	meshes_[name]->indexOffset = indexOffset;
	meshes_[name]->vertexOffset = vertexOffset;
}

void VaoWrapper::bind() const
{
	glBindVertexArray(vaoId_);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void VaoWrapper::unbind() const
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

const BasicMesh* VaoWrapper::operator[](const std::string& name) const
{
	ENSURES(meshes_.count(name) > 0);
	return meshes_.at(name);
}
