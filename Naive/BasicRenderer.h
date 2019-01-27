/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace Naive
	{
		class BasicRenderer : public Shared::RendererBase<BasicMesh> {
			using Base = Shared::RendererBase<BasicMesh>;
		public:
			BasicRenderer(ShaderPipeline* pipeline);
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
		};
	}
}
