#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace AZDO
	{
		struct DrawElementsCommand {
			GLuint indexCount; // Num indices of mesh
			GLuint instanceCount; // Number of instances of the mesh
			GLuint firstIndex; // The offset of the indices in the index buffer of the mesh
			GLuint baseVertex; // The offset of the vertices in the vertex buffer of the mesh
			GLuint baseInstance; // Encodes the index in to an array of instance data, the final index is gl_BaseInstance + gl_InstanceID
		};

		struct InstanceData {
			glm::mat4 model;
			glm::mat4 mvp;
		};

		template<typename InstType>
		class AbstractRenderer : public Shared::RendererBase {
		public:
			AbstractRenderer(ShaderPipeline* pipeline)
				: Shared::RendererBase(pipeline) {
				glGenBuffers(1, &instanceDataBuffer_);
			}
			~AbstractRenderer() {
				for (auto& it : meshes_) {
					for (auto& mesh : it.second) {
						for (auto& inst : mesh.second) {
							SAFE_DELETE(inst);
						}
						SAFE_DELETE(mesh.first);
					}
				}
				glDeleteBuffers(1, &instanceDataBuffer_);
			}
			void addMesh(GLuint vaoId, BasicMesh* mesh, InstType* instance) {
				meshes_[vaoId][mesh].push_back(instance);
			}
		protected:
			// VaoId |-> multiple of (mesh |-> multiple of (instances))
			std::map<GLuint, std::map<BasicMesh*, std::vector<InstType*>>> meshes_;

			GLuint instanceDataBuffer_;
			std::vector<DrawElementsCommand> commandBuffer_;
		};
	}
}
