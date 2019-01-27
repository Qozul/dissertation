/// Author: Ralph Ridley
/// Date: 27/01/18
#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace Naive
	{
		class LoopRenderer : public Shared::RendererBase<BasicMesh> {
			using Base = Shared::RendererBase<BasicMesh>;
		public:
			LoopRenderer(ShaderPipeline* pipeline);
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;

		private:
			void updateTransform(QZL::Shared::Transform& transform);

			static const float kRotationSpeed;
		};
	}
}