/// Author: Ralph Ridley
/// Date: 31/1/19
#include "TexturedRenderer.h"
#include "VaoWrapper.h"
#include "Texture.h"
#include "RendererStorage.h"

using namespace QZL;
using namespace QZL::AZDO;

TexturedRenderer::TexturedRenderer(ShaderPipeline* pipeline, VaoWrapper* vao, const glm::mat4* viewMatrix)
	: Base(pipeline, vao), viewMatrix_(viewMatrix)
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
	setupInstanceDataBuffer();
	auto instPtr = renderStorage_->instanceData();
	for (size_t i = 0; i < renderStorage_->instanceCount(); ++i) {
		glm::mat4 model = (instPtr + i)->transform.toModelMatrix();
		instanceDataBufPtr_[i] = {
			model, Shared::kProjectionMatrix * *viewMatrix_ * model
		};
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureBuffer_);
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, renderStorage_->textureCount() * sizeof(TextureDataWrapper), 0, flags);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, textureBuffer_);
	texBufPtr_ = static_cast<TextureDataWrapper*>(glMapNamedBufferRange(textureBuffer_, 0,
		renderStorage_->textureCount() * sizeof(TextureDataWrapper), flags));
	memcpy(texBufPtr_, renderStorage_->textureData(), renderStorage_->textureCount() * sizeof(TextureDataWrapper));
}

void TexturedRenderer::doFrame(const glm::mat4& viewMatrix)
{
	bindInstanceDataBuffer();
	pipeline_->use();
	renderStorage_->vao()->bind();
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer_);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, renderStorage_->meshCount() * sizeof(DrawElementsCommand), renderStorage_->meshData(), GL_DYNAMIC_DRAW);

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, nullptr, renderStorage_->meshCount(), 0);
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	renderStorage_->vao()->unbind();
	pipeline_->unuse();
}
