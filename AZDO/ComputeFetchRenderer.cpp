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
	: Base(pipeline, vao), computePipeline_(new ShaderPipeline("AZDOCompute2")), compBufPtr_(nullptr)
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
	setupInstanceDataBuffer();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeBuffer_);
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, renderStorage_->instanceCount() * sizeof(MeshInstance), renderStorage_->instanceData(), flags);
	compBufPtr_ = glMapNamedBufferRange(computeBuffer_, 0, renderStorage_->instanceCount() * sizeof(MeshInstance), flags);
}

void ComputeFetchRenderer::doFrame(const glm::mat4& viewMatrix)
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

	auto result = glClientWaitSync(fence_, GL_SYNC_FLUSH_COMMANDS_BIT, std::numeric_limits<GLuint64>::max());
	ENSURES(result != GL_WAIT_FAILED);
	memcpy(renderStorage_->instanceData(), compBufPtr_, renderStorage_->instanceCount() * sizeof(MeshInstance));
}

void ComputeFetchRenderer::computeTransform(const glm::mat4& viewMatrix)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeBuffer_);
	computePipeline_->use();
	glUniform1f(computePipeline_->getUniformLocation("uRotationAmount"), kRotationSpeed);
	glUniformMatrix4fv(computePipeline_->getUniformLocation("uViewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(computePipeline_->getUniformLocation("uProjMatrix"), 1, GL_FALSE, glm::value_ptr(Shared::kProjectionMatrix));
	glDispatchCompute(renderStorage_->instanceCount(), 1, 1);
	computePipeline_->unuse();

	fence_ = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	if (fence_ == 0)
		glFinish();
}
