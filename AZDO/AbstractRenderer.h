/// Author: Ralph Ridley
/// Date: 02/02/19
/// Purpose: Define shared behaviour of AZDO renderers
#pragma once
#include "../Shared/RendererBase.h"
#include "Mesh.h"

namespace QZL
{
	namespace AZDO
	{
		class VaoWrapper;

		struct DrawElementsCommand {
			GLuint indexCount; // Num indices of mesh
			GLuint instanceCount; // Number of instances of the mesh
			GLuint firstIndex; // The offset of the indices in the index buffer of the mesh
			GLuint baseVertex; // The offset of the vertices in the vertex buffer of the mesh
			GLuint baseInstance; // Encodes the index in to an array of instance data, the final index is gl_BaseInstance + gl_InstanceID
			// Note that baseInstance should be 0 if gl version is less than 4.2 TODO
			DrawElementsCommand(GLuint idxCount, GLuint instCount, GLuint firstIdx, GLuint baseVert, GLuint baseInst)
				: indexCount(idxCount), instanceCount(instCount), firstIndex(firstIdx), baseVertex(baseVert), baseInstance(baseInst) {}
		};

		struct InstanceData {
			glm::mat4 model;
			glm::mat4 mvp;
		};

		template<typename InstType>
		class AbstractRenderer : public Shared::RendererBase {
		public:
			AbstractRenderer(ShaderPipeline* pipeline)
				: Shared::RendererBase(pipeline), instanceDataBufPtr_(nullptr) {
				glGenBuffers(1, &instanceDataBuffer_);
				glGenBuffers(1, &commandBuffer_);
			}
			~AbstractRenderer() {
				for (auto& it : meshes_) {
					for (auto& mesh : it.second) {
						for (auto& inst : mesh.second) {
							SAFE_DELETE(inst);
						}
					}
				}
				if (instanceDataBufPtr_)
					glUnmapNamedBuffer(instanceDataBuffer_);
				glDeleteBuffers(1, &instanceDataBuffer_);
				glDeleteBuffers(1, &commandBuffer_);
			}
			virtual void initialise() = 0;
			virtual void doFrame(const glm::mat4& viewMatrix) = 0;
			void setupInstanceDataBuffer() {
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceDataBuffer_);
				GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT;
				size_t numInstances = 0;
				for (auto& it : meshes_) {
					for (auto& mesh : it.second) {
						numInstances += mesh.second.size();
					}
				}
				glBufferStorage(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(InstanceData), 0, flags);
				instanceDataBufPtr_ = static_cast<InstanceData*>(glMapNamedBufferRange(instanceDataBuffer_, 0, numInstances * sizeof(InstanceData), flags));
			}
			void bindInstanceDataBuffer() {
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instanceDataBuffer_);
			}
			void addMesh(VaoWrapper* vao, std::string meshName, InstType* instance) {
				meshes_[vao][meshName].push_back(instance);
			}
		protected:
			// VaoId |-> multiple of (mesh name |-> multiple of (instances))
			std::map<VaoWrapper*, std::map<std::string, std::vector<InstType*>>> meshes_;
			size_t totalInstances_;
			size_t totalCommands_;
			//std::map<VaoWrapper*, std::vector<std::pair<std::string, std::vector<InstType*>>>> meshes_;
			// RenderStorage* renderStorage_;

			GLuint instanceDataBuffer_;
			InstanceData* instanceDataBufPtr_;
			GLuint commandBuffer_;
			std::vector<DrawElementsCommand> commandBufferClient_;
		};
	}
}
