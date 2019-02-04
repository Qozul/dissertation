/// Author: Ralph Ridley
/// Date: 31/1/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace AZDO
	{
		struct TextureData;

		class TexturedRenderer : public AbstractRenderer<TexturedMeshInstance> {
			using Base = AbstractRenderer<TexturedMeshInstance>;
		public:
			TexturedRenderer(ShaderPipeline* pipeline);
			~TexturedRenderer();
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
		private:
			GLuint textureBuffer_;
			TextureData* texBufPtr_;
		};
	}
}