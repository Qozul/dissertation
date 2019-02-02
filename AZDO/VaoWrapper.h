/// Author: Ralph Ridley
/// Date: 02/02/19
/// Purpose: Contain and define operations on a vertex array
#pragma once
#include "../Shared/Vertex.h"
#include "../Shared/Mesh.h"

namespace QZL
{
	namespace AZDO
	{
		struct BasicMesh;

		class VaoWrapper {
			using Index = GLushort;
		public:
			VaoWrapper();
			~VaoWrapper();
			void commit();
			size_t addVertices(Shared::Vertex* data, const size_t size);
			size_t addIndices(Index* data, const size_t size);
			void emplaceMesh(std::string name, GLuint indexCount, GLuint indexOffset, GLuint vertexOffset);
			template<typename InstType>
			InstType* getNewMeshInstance(std::string name);
			void bind() const;
			void unbind() const;
			
			const BasicMesh* operator[](const std::string& name) const;

			bool isCommitted() {
				return isCommitted_;
			}
			GLuint getVaoId() {
				return vaoId_;
			}
			bool contains(std::string name) {
				return meshes_.count(name) > 0;
			}
		private:
			bool isCommitted_;
			GLuint vaoId_;
			GLuint vboId_;
			GLuint iboId_;
			std::map<std::string, BasicMesh*> meshes_;
			std::vector<Index> indices_;
			std::vector<Shared::Vertex> vertices_;
		};
		template<typename InstType>
		inline InstType* VaoWrapper::getNewMeshInstance(std::string name)
		{
			ENSURES(meshes_.count(name) > 0);
			return makeMeshInstance<InstType>(name, *meshes_[name]);
		}
	}
}
