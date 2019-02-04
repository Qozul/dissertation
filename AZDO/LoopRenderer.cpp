/// Author: Ralph Ridley
/// Date: 31/01/19

#include "LoopRenderer.h"
#include "VaoWrapper.h"

using namespace QZL;
using namespace QZL::AZDO;

LoopRenderer::LoopRenderer(ShaderPipeline* pipeline)
	: Base(pipeline)
{
}

void LoopRenderer::initialise()
{
	totalCommands_ = 0;
	for (auto& it : meshes_) {
		for (auto& mesh : it.second) {
			++totalCommands_;
			for (int i = 0; i < mesh.second.size(); ++i) {
				auto inst = mesh.second[i];
				inst->transform.position = glm::vec3(-4.0f + i * 0.5f, 2.0f, 0.0f);
				inst->transform.setScale(0.2f);
			}
		}
	}
	setupInstanceDataBuffer();
	commandBufferClient_.reserve(totalCommands_);
}

void LoopRenderer::doFrame(const glm::mat4& viewMatrix)
{
	for (const auto& it : meshes_) {
		for (const auto& it2 : it.second) {
			for (const auto& inst : it2.second) {
				inst->transform.angle = inst->transform.angle + 0.1f;
			}
		}
	}
	bindInstanceDataBuffer();
	pipeline_->use();
	for (const auto& it : meshes_) {
		// first = VaoWrapper, second = string |-> instances
		it.first->bind();
		GLuint instanceCount = 0;
		for (const auto& it2 : it.second) {
			// Build command buffer
			const BasicMesh* mesh = (*it.first)[it2.first];
			commandBufferClient_.emplace_back(mesh->indexCount, it2.second.size(), mesh->indexOffset, mesh->vertexOffset, instanceCount);
			instanceCount += it2.second.size();
			for (int i = 0; i < it2.second.size(); ++i) {
				auto inst = it2.second[i];
				// Build instance data buffer
				glm::mat4 model = inst->transform.toModelMatrix();
				instanceDataBufPtr_[i] = {
					model, Shared::kProjectionMatrix * viewMatrix * model
				};
			}
		}
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer_);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, (commandBufferClient_.size()) * sizeof(DrawElementsCommand), commandBufferClient_.data(), GL_DYNAMIC_DRAW);

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, nullptr, commandBufferClient_.size(), 0);
		glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		it.first->unbind();
	}
	pipeline_->unuse();
	commandBufferClient_.clear();
}
