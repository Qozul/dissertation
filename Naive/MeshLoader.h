/// Author: Ralph Ridley
/// Date: 15/01/19
/// Purpose: Encapsulate and support loading meshes from .obj files.
#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace Naive
	{
		struct BasicMesh;

		class MeshLoader {
		public:
			MeshLoader() {};
			~MeshLoader() {
				glDeleteBuffers(static_cast<GLsizei>(vertexBuffers_.size()), vertexBuffers_.data());
				glDeleteBuffers(static_cast<GLsizei>(indexBuffers_.size()), indexBuffers_.data());
				glDeleteVertexArrays(static_cast<GLsizei>(vertexArrays_.size()), vertexArrays_.data());
			};
			MeshLoader(const MeshLoader& other) = delete;
			BasicMesh* loadMesh(const std::string& meshName);

		private:
			std::vector<GLuint> vertexBuffers_;
			std::vector<GLuint> indexBuffers_;
			std::vector<GLuint> vertexArrays_;

			// Map mesh name to vao id and index count
			std::map<std::string, std::pair<GLuint, GLuint>> loadedMeshes_;

			static const std::string kPath;
			static const std::string kExt;
		};
	}
}