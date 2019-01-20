/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once

#include "ShaderPipeline.h"
#include "Mesh.h"
#include "Texture.h"

namespace QZL
{
	namespace Shared
	{
		template<typename M>
		class RendererBase {
		public:
			// pipeline must by dynamically allocated
			RendererBase(ShaderPipeline* pipeline) : pipeline_(pipeline) {};
			virtual ~RendererBase() {
				delete pipeline_;
			}
			virtual void doFrame(const glm::mat4& viewMatrix) = 0;
			void addMesh(GLuint id, M* mesh) {
				meshes_[id].push_back(mesh);
			}
			void addMesh(GLuint id, std::vector<M*> meshes) {
				std::copy(std::begin(meshes), std::end(meshes), std::back_inserter(meshes_[id]));
			}
		protected:
			ShaderPipeline* pipeline_;
			// VAO id, mesh ptr not owned here. Use dummy vao id for naive.
			std::map<GLuint, std::vector<M*>> meshes_;
		};
	}
}
