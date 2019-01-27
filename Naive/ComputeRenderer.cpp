#include "ComputeRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

const float ComputeRenderer::kRotationSpeed = 0.2f;

ComputeRenderer::ComputeRenderer(ShaderPipeline* pipeline)
	: Base(pipeline), computePipeline_(new ShaderPipeline("NaiveCompute"))
{
	// SSBO for compute write
	glGenBuffers(1, &computeBuffer_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeBuffer_);
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat), 0, flags);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeBuffer_);
	compBufPtr_ = glMapNamedBufferRange(computeBuffer_, 0, sizeof(GLfloat), flags);
	QZL::Shared::checkGLError();
}

ComputeRenderer::~ComputeRenderer()
{
	glDeleteBuffers(1, &computeBuffer_);
	glUnmapNamedBuffer(computeBuffer_);
	SAFE_DELETE(computePipeline_)
}

void ComputeRenderer::doFrame(const glm::mat4& viewMatrix)
{
	computePipeline_->use();
	for (auto& mesh : meshes_[0]) {
		DEBUG_OUT(mesh->transform.angle);
		computeTransform(mesh->transform);
		DEBUG_OUT(mesh->transform.angle);
	}
	computePipeline_->unuse();
	pipeline_->use();
	for (const auto& mesh : meshes_[0]) {
		GLint loc = pipeline_->getUniformLocation("uMVP");
		glm::mat4 model = mesh->transform.toModelMatrix();
		glm::mat4 mvp = Shared::kProjectionMatrix * viewMatrix * model;
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

		glBindVertexArray(mesh->vaoId);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}
	glBindVertexArray(0);
	pipeline_->unuse();
}

void ComputeRenderer::computeTransform(QZL::Shared::Transform& transform)
{
	GLint rotLoc = computePipeline_->getUniformLocation("uRotationAmount");
	glUniform1f(rotLoc, kRotationSpeed);
	GLint angLoc = computePipeline_->getUniformLocation("uCurrentAngle");
	glUniform1f(angLoc, transform.angle);

	glDispatchCompute(1, 1, 1);

	glFinish();
	memcpy(&transform.angle, compBufPtr_, sizeof(GLfloat));
	QZL::Shared::checkGLError();
}
