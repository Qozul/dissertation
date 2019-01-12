/// Author: Ralph Ridley
/// Date: 10/1/19

#include "NaiveTexturedRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

TexturedRenderer::TexturedRenderer(ShaderPipeline* pipeline)
	: AbstractRenderer(pipeline)
{
}

void TexturedRenderer::doFrame()
{
	pipeline_->use();
	for (const auto& it : texturedMeshes_) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, it.first->id);
		for (const auto& mesh : it.second) {
			glBindVertexArray(mesh->vaoId);
			glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_SHORT, 0);
		}
	}
	glBindVertexArray(0);
	pipeline_->unuse();
}
