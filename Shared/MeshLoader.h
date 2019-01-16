/// Author: Ralph Ridley
/// Date: 15/01/19
/// Purpose: Encapsulate and support loading meshes from .obj files.
#pragma once
#include "Mesh.h"

namespace QZL
{
	namespace Shared
	{
		class MeshLoader {
			struct Vertex {
				GLfloat x, y, z;
				GLfloat u, v;
				GLfloat nx, ny, nz;
			};
		public:
			MeshLoader() {};
			~MeshLoader() {
				glDeleteBuffers(static_cast<GLsizei>(vertexBuffers_.size()), vertexBuffers_.data());
				glDeleteBuffers(static_cast<GLsizei>(indexBuffers_.size()), indexBuffers_.data());
				glDeleteVertexArrays(static_cast<GLsizei>(vertexArrays_.size()), vertexArrays_.data());
			};
			MeshLoader(const MeshLoader& other) = delete;
			Naive::BasicMesh* loadNaiveMesh(const std::string& meshName);

		private:
			std::vector<GLuint> vertexBuffers_;
			std::vector<GLuint> indexBuffers_;
			std::vector<GLuint> vertexArrays_;

			static const std::string kPath;
			static const std::string kExt;
		};
	}
}