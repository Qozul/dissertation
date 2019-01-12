/// Author: Ralph Ridley
/// Date: 31/12/18

#include "NaiveBasicRenderer.h"

using namespace QZL;
using namespace QZL::Naive;

BasicRenderer::BasicRenderer(ShaderPipeline* pipeline)
	: AbstractRenderer(pipeline)
{
}

void BasicRenderer::doFrame()
{
	pipeline_->use();
	for (const auto& mesh : meshes_) {
		glBindVertexArray(mesh->vaoId);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_SHORT, 0);
	}
	glBindVertexArray(0);
	pipeline_->unuse();
}
