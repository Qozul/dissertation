/// Author: Ralph Ridley
/// Date: 31/1/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace AZDO
	{
		struct TextureDataWrapper;
		class VaoWrapper;

		class TexturedRenderer : public AbstractRenderer<TexturedMeshInstance> {
			using Base = AbstractRenderer<TexturedMeshInstance>;
		public:
			TexturedRenderer(ShaderPipeline* pipeline, VaoWrapper* vao, const glm::mat4* viewMatrix);
			~TexturedRenderer();
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
		private:
			GLuint textureBuffer_;
			TextureDataWrapper* texBufPtr_;
			const glm::mat4* viewMatrix_;
		};
	}
}