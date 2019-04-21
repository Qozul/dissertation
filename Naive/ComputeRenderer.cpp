#include "ComputeRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

const float ComputeRenderer::kRotationSpeed = 0.11f;

ComputeRenderer::ComputeRenderer(ShaderPipeline* pipeline)
	: Base(pipeline), computePipeline_(new ShaderPipeline("NaiveCompute")), compBufPtr_(nullptr), rot_(0.0f)
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
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeBuffer_);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(ElementData), NULL, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeBuffer_);
}

void ComputeRenderer::doFrame(const glm::mat4& viewMatrix)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeBuffer_);
	for (auto& mesh : meshes_) {
		rot_ += kRotationSpeed;
		computeTransform(viewMatrix, *mesh);
		pipeline_->use();
		glBindVertexArray(mesh->vaoId);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		pipeline_->unuse();
	}
	glBindVertexArray(0);
}

void ComputeRenderer::computeTransform(const glm::mat4& viewMatrix, BasicMesh& mesh)
{
	computePipeline_->use();
	glUniform1fv(computePipeline_->getUniformLocation("uTransform"), 10, mesh.transform.data());
	glUniform1f(computePipeline_->getUniformLocation("uRotationAmount"), rot_);
	
	glUniformMatrix4fv(computePipeline_->getUniformLocation("uProjMatrix"), 1, GL_FALSE, glm::value_ptr(Shared::kProjectionMatrix));
	glUniformMatrix4fv(computePipeline_->getUniformLocation("uViewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glDispatchCompute(1, 1, 1);
	computePipeline_->unuse();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
