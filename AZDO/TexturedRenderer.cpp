/// Author: Ralph Ridley
/// Date: 31/1/19
#include "TexturedRenderer.h"
#include "VaoWrapper.h"
#include "Texture.h"

using namespace QZL;
using namespace QZL::AZDO;

TexturedRenderer::TexturedRenderer(ShaderPipeline* pipeline)
	: Base(pipeline)
{
	glGenBuffers(1, &textureBuffer_);
}

TexturedRenderer::~TexturedRenderer()
{
	if (texBufPtr_)
		glUnmapNamedBuffer(textureBuffer_);
	glDeleteBuffers(1, &textureBuffer_);
}

void TexturedRenderer::initialise()
{
	totalCommands_ = 0;
	for (auto& it : meshes_) {
		for (auto& mesh : it.second) {
			++totalCommands_;
			for (int i = 0; i < mesh.second.size(); ++i) {
				auto inst = mesh.second[i];
				inst->transform.position = glm::vec3(-4.0f + i * 0.5f, -1.0f, 0.0f);
				inst->transform.setScale(0.2f);
			}
		}
	}
	setupInstanceDataBuffer();
	commandBufferClient_.reserve(totalCommands_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureBuffer_);
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, totalInstances_ * sizeof(TextureData), 0, flags);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, textureBuffer_);
	// Persistantly map
	texBufPtr_ = static_cast<TextureData*>(glMapNamedBufferRange(textureBuffer_, 0, totalInstances_ * sizeof(TextureData), flags));
}

void TexturedRenderer::doFrame(const glm::mat4& viewMatrix)
{
	bindInstanceDataBuffer();
	pipeline_->use();
	for (const auto& it : meshes_) {
		// first = VaoWrapper, second = string |-> instances
		it.first->bind();
		GLuint instanceCount = 0;
		for (const auto& it2 : it.second) {
			// Build command buffer
			const BasicMesh* mesh = (*it.first)[it2.first];
			commandBufferClient_.emplace_back(mesh->indexCount, it2.second.size(), mesh->indexOffset, mesh->vertexOffset, instanceCount);
			instanceCount += it2.second.size();
			for (int i = 0; i < it2.second.size(); ++i) {
				auto inst = it2.second[i];
				// Build instance data buffer
				glm::mat4 model = inst->transform.toModelMatrix();
				instanceDataBufPtr_[i] = {
					model, Shared::kProjectionMatrix * viewMatrix * model
				};
				texBufPtr_[i] = inst->texture->data();
			}
		}
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer_);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, (commandBufferClient_.size()) * sizeof(DrawElementsCommand), commandBufferClient_.data(), GL_DYNAMIC_DRAW);

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, nullptr, commandBufferClient_.size(), 0);
		glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		it.first->unbind();
	}
	pipeline_->unuse();
	commandBufferClient_.clear();
}
