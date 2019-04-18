#include "LoopRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

const float LoopRenderer::kRotationSpeed = 0.1f;

LoopRenderer::LoopRenderer(ShaderPipeline* pipeline)
	: Base(pipeline)
{
}

void LoopRenderer::initialise()
{
}

void LoopRenderer::doFrame(const glm::mat4& viewMatrix)
{
	pipeline_->use();
	for (const auto& mesh : meshes_) {
		updateTransform(mesh->transform);
		glm::mat4 model = mesh->transform.toModelMatrix();
		glm::mat4 mvp = Shared::kProjectionMatrix * viewMatrix * model;
		glUniformMatrix4fv(pipeline_->getUniformLocation("uModelMat"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(pipeline_->getUniformLocation("uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));

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

void LoopRenderer::updateTransform(QZL::Shared::Transform& transform)
{
	transform.angle = transform.angle + kRotationSpeed;
}
