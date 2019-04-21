#include "ComputeFetchRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

const float ComputeFetchRenderer::kRotationSpeed = 0.11f;

ComputeFetchRenderer::ComputeFetchRenderer(ShaderPipeline* pipeline)
	: Base(pipeline), computePipeline_(new ShaderPipeline("NaiveCompute2")), compTransBufPtr_(nullptr)
{
	// SSBO for compute write
	glGenBuffers(1, &computeBuffer_);
	glGenBuffers(1, &computeTransBuffer_);
}

ComputeFetchRenderer::~ComputeFetchRenderer()
{
	if (compTransBufPtr_)
		glUnmapNamedBuffer(computeTransBuffer_);
	glDeleteBuffers(1, &computeBuffer_);
	glDeleteBuffers(1, &computeTransBuffer_);
	SAFE_DELETE(computePipeline_)
}

void ComputeFetchRenderer::initialise()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeBuffer_);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(ElementData), NULL, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeBuffer_);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeTransBuffer_);
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(Shared::Transform), NULL, flags);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeTransBuffer_);
	compTransBufPtr_ = glMapNamedBufferRange(computeTransBuffer_, 0, sizeof(Shared::Transform), flags);
	QZL::Shared::checkGLError();
}

void ComputeFetchRenderer::doFrame(const glm::mat4& viewMatrix)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeBuffer_);
	for (auto& mesh : meshes_) {
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
		pipeline_->unuse();

		auto result = glClientWaitSync(fence_, GL_SYNC_FLUSH_COMMANDS_BIT, std::numeric_limits<GLuint64>::max());
		ENSURES(result != GL_WAIT_FAILED);
		memcpy(&mesh->transform, compTransBufPtr_, sizeof(Shared::Transform));
	}
	glBindVertexArray(0);
}

void ComputeFetchRenderer::computeTransform(const glm::mat4& viewMatrix, BasicMesh& mesh)
{
	computePipeline_->use();
	memcpy(compTransBufPtr_, &mesh.transform, sizeof(Shared::Transform));

	glUniformMatrix4fv(computePipeline_->getUniformLocation("uProjMatrix"), 1, GL_FALSE, glm::value_ptr(Shared::kProjectionMatrix));
	glUniformMatrix4fv(computePipeline_->getUniformLocation("uViewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glDispatchCompute(1, 1, 1);
	computePipeline_->unuse();
	fence_ = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	if (fence_ == 0)
		glFinish();
}
