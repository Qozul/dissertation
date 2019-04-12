/// Author: Ralph Ridley
/// Date: 31/01/19
#include "ComputeRenderer.h"
#include "VaoWrapper.h"
#include "RendererStorage.h"

using namespace QZL;
using namespace QZL::AZDO;

const float ComputeRenderer::kRotationSpeed = 0.11f;

ComputeRenderer::ComputeRenderer(ShaderPipeline* pipeline, VaoWrapper* vao)
	: Base(pipeline, vao), computePipeline_(new ShaderPipeline("AZDOCompute2")), compBufPtr_(nullptr)
{
	glGenBuffers(1, &computeBuffer_);
}

ComputeRenderer::~ComputeRenderer()
{
	if (compBufPtr_)
		glUnmapNamedBuffer(computeBuffer_);
	glDeleteBuffers(1, &computeBuffer_);
	SAFE_DELETE(computePipeline_)
}

void ComputeRenderer::initialise()
{
	auto instPtr = renderStorage_->instanceData();
	for (size_t i = 0; i < renderStorage_->instanceCount(); ++i) {
		(instPtr + i)->transform.position = glm::vec3(-4.0f + i * 0.5f, 0.0f, 0.0f);
		(instPtr + i)->transform.setScale(0.2f);
	}
	setupInstanceDataBuffer();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeBuffer_);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, renderStorage_->instanceCount() * sizeof(MeshInstance), renderStorage_->instanceData(), GL_MAP_WRITE_BIT);
}

void ComputeRenderer::doFrame(const glm::mat4& viewMatrix)
{
	bindInstanceDataBuffer();
	computeTransform(viewMatrix);
	pipeline_->use();
	renderStorage_->vao()->bind();
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer_);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, renderStorage_->meshCount() * sizeof(DrawElementsCommand), renderStorage_->meshData(), GL_DYNAMIC_DRAW);

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, nullptr, renderStorage_->meshCount(), 0);
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	renderStorage_->vao()->unbind();
	pipeline_->unuse();
}

void ComputeRenderer::computeTransform(const glm::mat4& viewMatrix)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeBuffer_);
	computePipeline_->use();
	glUniform1f(computePipeline_->getUniformLocation("uRotationAmount"), kRotationSpeed);
	glUniformMatrix4fv(computePipeline_->getUniformLocation("uViewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(computePipeline_->getUniformLocation("uProjMatrix"), 1, GL_FALSE, glm::value_ptr(Shared::kProjectionMatrix));
	glDispatchCompute(renderStorage_->instanceCount(), 1, 1);
	computePipeline_->unuse();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
