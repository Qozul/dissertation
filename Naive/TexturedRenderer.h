/// Author: Ralph Ridley
/// Date: 10/1/19
#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace Naive
	{
		struct TexturedBasicMesh;

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
			std::map<GLuint, std::vector<TexturedBasicMesh*>> meshes_;
		};
	}
}