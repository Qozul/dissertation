/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace Naive
	{
		class BasicRenderer : public AbstractRenderer {
			using Base = AbstractRenderer;
		public:
			BasicRenderer(ShaderPipeline* pipeline);
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
		};
	}
}
