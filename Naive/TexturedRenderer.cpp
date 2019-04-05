/// Author: Ralph Ridley
/// Date: 10/1/19

#include "TexturedRenderer.h"
#include "Mesh.h"

using namespace QZL;
using namespace QZL::Naive;

TexturedRenderer::TexturedRenderer(ShaderPipeline* pipeline)
	: Base(pipeline)
{
}

TexturedRenderer::~TexturedRenderer()
{
	for (auto& it : meshes_) {
		for (TexturedBasicMesh* mesh : it.second) {
			SAFE_DELETE(mesh);
		}
	}
}

void TexturedRenderer::initialise()
{
	meshes_[1][0]->transform.position = glm::vec3(-2.0f, 2.0f, 0.0f);
	meshes_[1][0]->transform.setScale(0.7f);
	meshes_[2][0]->transform.position = glm::vec3(-2.0f, 2.0f, 0.0f);
	meshes_[2][0]->transform.setScale(0.7f);
}

void TexturedRenderer::doFrame(const glm::mat4& viewMatrix)
{
	pipeline_->use();
	for (const auto& it : meshes_) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, it.first);
		for (const auto& mesh : it.second) {
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
	}
	glBindVertexArray(0);
	pipeline_->unuse();
}
