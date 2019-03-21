/// Author: Ralph Ridley
/// Date: 31/01/19
#include "ComputeFetchRenderer.h"
#include "VaoWrapper.h"
#include "../Shared/Transform.h"
#include "RendererStorage.h"

using namespace QZL;
using namespace QZL::AZDO;

const float ComputeFetchRenderer::kRotationSpeed = 0.11f;

ComputeFetchRenderer::ComputeFetchRenderer(ShaderPipeline* pipeline, VaoWrapper* vao)
	: Base(pipeline, vao), computePipeline_(new ShaderPipeline("AZDOCompute")), compBufPtr_(nullptr)
{
	glGenBuffers(1, &computeBuffer_);
}

ComputeFetchRenderer::~ComputeFetchRenderer()
{
	if (compBufPtr_)
		glUnmapNamedBuffer(computeBuffer_);
	glDeleteBuffers(1, &computeBuffer_);
	SAFE_DELETE(computePipeline_)
}

void ComputeFetchRenderer::initialise()
{
	auto instPtr = renderStorage_->instanceData();
	for (size_t i = 0; i < renderStorage_->instanceCount(); ++i) {
		(instPtr + i)->transform.position = glm::vec3(-4.0f + i * 0.5f, 0.0f, 0.0f);
		(instPtr + i)->transform.setScale(0.2f);
	}
	setupInstanceDataBuffer();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeBuffer_);
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, renderStorage_->instanceCount() * sizeof(MeshInstance), 0, flags);
	compBufPtr_ = glMapNamedBufferRange(computeBuffer_, 0, renderStorage_->instanceCount() * sizeof(MeshInstance), flags);
}

void ComputeFetchRenderer::doFrame(const glm::mat4& viewMatrix)
{
	bindInstanceDataBuffer();
	computeTransform();
	pipeline_->use();
	renderStorage_->vao()->bind();
	auto instPtr = renderStorage_->instanceData();
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

void ComputeFetchRenderer::computeTransform()
{
	memcpy(compBufPtr_, renderStorage_->instanceData(), renderStorage_->instanceCount() * sizeof(MeshInstance));
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeBuffer_);
	computePipeline_->use();
	GLint rotLoc = computePipeline_->getUniformLocation("uRotationAmount");
	glUniform1f(rotLoc, kRotationSpeed);
	glDispatchCompute(renderStorage_->instanceCount(), 1, 1);
	computePipeline_->unuse();

	glFinish();
	memcpy(renderStorage_->instanceData(), compBufPtr_, renderStorage_->instanceCount() * sizeof(MeshInstance));
}
