/// Author: Ralph Ridley
/// Date: 31/01/19

#include "BasicRenderer.h"

using namespace QZL;
using namespace QZL::AZDO;

BasicRenderer::BasicRenderer(ShaderPipeline* pipeline)
	: Base(pipeline)
{
}

void BasicRenderer::initialise()
{
	for (auto& it : meshes_) {
		for (auto& mesh : it.second) {
			for (auto& inst : mesh.second) {
				inst->transform.position = glm::vec3(2.0f, 2.0f, 0.0f);
				inst->transform.setScale(0.7f);
			}
		}
	}
	commandBuffer_.reserve(meshes_.count);
}

void BasicRenderer::doFrame(const glm::mat4& viewMatrix)
{
	pipeline_->use();
	/*GLint loc0 = pipeline_->getUniformLocation("uModelMat");
	GLint loc1 = pipeline_->getUniformLocation("uMVP");
	glm::mat4 model = mesh->transform.toModelMatrix();
	glm::mat4 mvp = Shared::kProjectionMatrix * viewMatrix * model;
	glUniformMatrix4fv(loc0, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(loc1, 1, GL_FALSE, glm::value_ptr(mvp));*/

	for (const auto& it : meshes_) {
		glBindVertexArray(it.first);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		GLuint i = 0;
		for (const auto& mesh : it.second) {
			// Build command buffer
			commandBuffer_.emplace_back(mesh.first->indexCount, mesh.second.size(), mesh.first->indexOffset, mesh.first->vertexOffset, i);
			i += mesh.second.size();

			for (int i = 0; i < mesh.second.size(); ++i) {
				auto inst = mesh.second[i];
				// Build instance data buffer
				
			}
		}
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, &*commandBuffer_.begin(), commandBuffer_.size(), 0);
		glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}
	glBindVertexArray(0);
	pipeline_->unuse();
}
