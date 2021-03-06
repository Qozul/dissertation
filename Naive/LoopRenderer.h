/// Author: Ralph Ridley
/// Date: 27/01/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace Naive
	{
		class LoopRenderer : public AbstractRenderer {
			using Base = AbstractRenderer;
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