/// Author: Ralph Ridley
/// Date: 02/02/19
/// Purpose: Define shared behaviour of AZDO renderers
#pragma once
#include "../Shared/RendererBase.h"
#include "RendererStorage.h"
#include "Mesh.h"
#include "DrawElementsCommand.h"

namespace QZL
{
	namespace AZDO
	{
		class VaoWrapper;

		struct InstanceData {
			glm::mat4 model;
			glm::mat4 mvp;
		};

		template<typename InstType>
		class AbstractRenderer : public Shared::RendererBase {
		public:
			AbstractRenderer(ShaderPipeline* pipeline, VaoWrapper* vao)
				: Shared::RendererBase(pipeline), instanceDataBufPtr_(nullptr), renderStorage_(new RenderStorage(vao)) 
			{
				glGenBuffers(1, &instanceDataBuffer_);
				glGenBuffers(1, &commandBuffer_);
			}
			~AbstractRenderer() 
			{
				SAFE_DELETE(renderStorage_)
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
				glBufferStorage(GL_SHADER_STORAGE_BUFFER, renderStorage_->instanceCount() * sizeof(InstanceData), 0, flags);
				instanceDataBufPtr_ = static_cast<InstanceData*>(glMapNamedBufferRange(instanceDataBuffer_, 0, 
					renderStorage_->instanceCount() * sizeof(InstanceData), flags));
			}
			void bindInstanceDataBuffer() {
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instanceDataBuffer_);
			}
			void addMesh(const std::string& meshName, BasicMesh* instance);
			void addMeshInstance(const std::string& meshName, InstType* instance);
		protected:
			RenderStorage* renderStorage_;

			GLuint instanceDataBuffer_;
			InstanceData* instanceDataBufPtr_;
			GLuint commandBuffer_;
		};
		template<typename InstType>
		inline void AbstractRenderer<InstType>::addMesh(const std::string& meshName, BasicMesh* mesh)
		{
			renderStorage_->addMesh(meshName, mesh);
		}
		template<typename InstType>
		inline void AbstractRenderer<InstType>::addMeshInstance(const std::string& meshName, InstType* instance)
		{
			renderStorage_->addInstance(meshName, instance);
		}
	}
}
