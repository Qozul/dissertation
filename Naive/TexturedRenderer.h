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
		class TexturedRenderer : public Shared::RendererBase<TexturedBasicMesh> {
			using Base = Shared::RendererBase<TexturedBasicMesh>;
		public:
			TexturedRenderer(ShaderPipeline* pipeline);
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
		};
	}
}