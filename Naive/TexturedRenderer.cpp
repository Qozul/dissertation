/// Author: Ralph Ridley
/// Date: 10/1/19

#include "TexturedRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

TexturedRenderer::TexturedRenderer(ShaderPipeline* pipeline)
	: Base(pipeline)
{
}

void TexturedRenderer::doFrame(const glm::mat4& viewMatrix)
{
	pipeline_->use();
	for (const auto& it : meshes_) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, it.first);
		for (const auto& mesh : it.second) {
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
	}
	glBindVertexArray(0);
	pipeline_->unuse();
}
