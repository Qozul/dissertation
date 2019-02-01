/// Author: Ralph Ridley
/// Date: 10/1/19
#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace Naive
	{
		// Note that TexturedRenderer uses base class meshes_ map GLuint key as proxy for texture id
		// rather than vao id
		class TexturedRenderer : public Shared::RendererBase {
			using Base = Shared::RendererBase;
		public:
			TexturedRenderer(ShaderPipeline* pipeline);
			~TexturedRenderer();
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
			void addMesh(GLuint texId, TexturedBasicMesh* mesh) {
				meshes_[texId].push_back(mesh);
			}
			void addMesh(GLuint texId, std::vector<TexturedBasicMesh*> meshes) {
				std::copy(std::begin(meshes), std::end(meshes), std::back_inserter(meshes_[texId]));
			}
		private:
			// VAO id, mesh ptr not owned here. Use dummy vao id for naive.
			std::map<GLuint, std::vector<TexturedBasicMesh*>> meshes_;
		};
	}
}