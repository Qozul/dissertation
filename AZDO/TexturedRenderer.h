/// Author: Ralph Ridley
/// Date: 31/1/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace AZDO
	{
		// Note that TexturedRenderer uses base class meshes_ map GLuint key as proxy for texture id
		// rather than vao id
		class TexturedRenderer : public AbstractRenderer<TexturedMeshInstance> {
			using Base = AbstractRenderer<TexturedMeshInstance>;
		public:
			TexturedRenderer(ShaderPipeline* pipeline);
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
		};
	}
}