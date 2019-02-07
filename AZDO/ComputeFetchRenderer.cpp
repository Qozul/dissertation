/// Author: Ralph Ridley
/// Date: 31/01/19
#include "ComputeFetchRenderer.h"
#include "VaoWrapper.h"

using namespace QZL;
using namespace QZL::AZDO;

const float ComputeFetchRenderer::kRotationSpeed = 0.11f;

ComputeFetchRenderer::ComputeFetchRenderer(ShaderPipeline* pipeline)
	: Base(pipeline), computePipeline_(new ShaderPipeline("AZDOCompute")), compBufPtr_(nullptr)
{
	glGenBuffers(1, &computeBuffer_);
	totalInstances_ = 0;
	totalCommands_ = 0;
}

ComputeFetchRenderer::~ComputeFetchRenderer()
{
	if (compBufPtr_)
		glUnmapNamedBuffer(computeBuffer_);
	glDeleteBuffers(1, &computeBuffer_);
	SAFE_DELETE(computePipeline_)
}

void ComputeFetchRenderer::initialise()
{
	totalInstances_ = 0;
	totalCommands_ = 0;
	for (auto& it : meshes_) {
		for (auto& mesh : it.second) {
			++totalCommands_;
			for (int i = 0; i < mesh.second.size(); ++i) {
				auto inst = mesh.second[i];
				inst->transform.position = glm::vec3(-4.0f + i * 0.5f, 0.0f, 0.0f);
				inst->transform.setScale(0.2f);
				++totalInstances_;
			}
		}
	}
	setupInstanceDataBuffer();
	commandBufferClient_.reserve(totalCommands_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeBuffer_);
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, totalInstances_ * sizeof(GLfloat), 0, flags);
	// Persistantly map
	compBufPtr_ = glMapNamedBufferRange(computeBuffer_, 0, totalInstances_ * sizeof(GLfloat), flags);
}

void ComputeFetchRenderer::doFrame(const glm::mat4& viewMatrix)
{
	bindInstanceDataBuffer();
	computeTransform();
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
				(instanceDataBufPtr_[i]) = {
					model, Shared::kProjectionMatrix * viewMatrix * model
				};
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

void ComputeFetchRenderer::computeTransform()
{
	for (auto& it : meshes_) {
		for (auto& mesh : it.second) {
			for (int i = 0; i < mesh.second.size(); ++i) {
				static_cast<GLfloat*>(compBufPtr_)[i] = mesh.second[i]->transform.angle;
				//memcpy(&static_cast<GLfloat*>(compBufPtr_)[i], &mesh.second[i]->transform.angle, sizeof(GLfloat));
			}
		}
	}
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeBuffer_);
	computePipeline_->use();
	GLint rotLoc = computePipeline_->getUniformLocation("uRotationAmount");
	glUniform1f(rotLoc, kRotationSpeed);
	glDispatchCompute(totalInstances_, 1, 1);
	computePipeline_->unuse();

	glFinish();
	for (auto& it : meshes_) {
		for (auto& mesh : it.second) {
			for (int i = 0; i < mesh.second.size(); ++i) {
				mesh.second[i]->transform.angle = static_cast<GLfloat*>(compBufPtr_)[i];
				//memcpy(&mesh.second[i]->transform.angle, static_cast<GLfloat*>(compBufPtr_) + sizeof(GLfloat) * i, sizeof(GLfloat));
			}
		}
	}
}
