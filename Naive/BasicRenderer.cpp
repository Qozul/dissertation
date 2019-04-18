/// Author: Ralph Ridley
/// Date: 31/12/18

#include "BasicRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

BasicRenderer::BasicRenderer(ShaderPipeline* pipeline, const glm::mat4* viewMatrix)
	: Base(pipeline), viewMatrix_(viewMatrix)
{
}

void BasicRenderer::initialise()
{
	idata_.reserve(meshes_.size());
	for (size_t i = 0; i < meshes_.size(); ++i) {
		glm::mat4 model = meshes_[i]->transform.toModelMatrix();
		glm::mat4 mvp = Shared::kProjectionMatrix * *viewMatrix_ * model;
		idata_.push_back({ model, mvp });
	}
}

void BasicRenderer::doFrame(const glm::mat4& viewMatrix)
{
	pipeline_->use();
	for (size_t i = 0; i < meshes_.size(); ++i) {
		GLint loc0 = pipeline_->getUniformLocation("uModelMat");
		GLint loc1 = pipeline_->getUniformLocation("uMVP");
		glUniformMatrix4fv(loc0, 1, GL_FALSE, glm::value_ptr(idata_[i].model));
		glUniformMatrix4fv(loc1, 1, GL_FALSE, glm::value_ptr(idata_[i].mvp));
		glBindVertexArray(meshes_[i]->vaoId);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawElements(GL_TRIANGLES, meshes_[i]->indexCount, GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}
	glBindVertexArray(0);
	pipeline_->unuse();
}
