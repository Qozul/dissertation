/// Author: Ralph Ridley
/// Date: 31/01/19

#include "LoopRenderer.h"
#include "VaoWrapper.h"
#include "RendererStorage.h"

using namespace QZL;
using namespace QZL::AZDO;

LoopRenderer::LoopRenderer(ShaderPipeline* pipeline, VaoWrapper* vao)
	: Base(pipeline, vao)
{
}

void LoopRenderer::initialise()
{
	auto instPtr = renderStorage_->instanceData();
	for (size_t i = 0; i < renderStorage_->instanceCount(); ++i) {
		(instPtr + i)->transform.position = glm::vec3(-4.0f + i * 0.5f, 2.0f, 0.0f);
		(instPtr + i)->transform.setScale(0.2f);
	}
	setupInstanceDataBuffer();
}

void LoopRenderer::doFrame(const glm::mat4& viewMatrix)
{
	auto instPtr = renderStorage_->instanceData();
	for (size_t i = 0; i < renderStorage_->instanceCount(); ++i) {
		(instPtr + i)->transform.angle = (instPtr + i)->transform.angle + 0.1f;
	}
	bindInstanceDataBuffer();
	pipeline_->use();
	renderStorage_->vao()->bind();

	for (size_t i = 0; i < renderStorage_->instanceCount(); ++i) {
		glm::mat4 model = (instPtr + i)->transform.toModelMatrix();
		instanceDataBufPtr_[i] = {
			model, Shared::kProjectionMatrix * viewMatrix * model
		};
	}
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer_);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, renderStorage_->meshCount() * sizeof(DrawElementsCommand), renderStorage_->meshData(), GL_DYNAMIC_DRAW);

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, nullptr, renderStorage_->meshCount(), 0);
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	renderStorage_->vao()->unbind();
	pipeline_->unuse();
}
