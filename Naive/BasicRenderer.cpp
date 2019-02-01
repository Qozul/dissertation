/// Author: Ralph Ridley
/// Date: 31/12/18

#include "BasicRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

BasicRenderer::BasicRenderer(ShaderPipeline* pipeline)
	: Base(pipeline)
{
}

void BasicRenderer::initialise()
{
	meshes_[0]->transform.position = glm::vec3(2.0f, 2.0f, 0.0f);
	meshes_[0]->transform.setScale(0.7f);
}

void BasicRenderer::doFrame(const glm::mat4& viewMatrix)
{
	pipeline_->use();
	for (const auto& mesh : meshes_) {
		GLint loc0 = pipeline_->getUniformLocation("uModelMat");
		GLint loc1 = pipeline_->getUniformLocation("uMVP");
		glm::mat4 model = mesh->transform.toModelMatrix();
		glm::mat4 mvp = Shared::kProjectionMatrix * viewMatrix * model;
		glUniformMatrix4fv(loc0, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(loc1, 1, GL_FALSE, glm::value_ptr(mvp));

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
