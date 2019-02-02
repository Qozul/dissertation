/// Author: Ralph Ridley
/// Date: 31/01/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace AZDO
	{
		class BasicRenderer : public AbstractRenderer<MeshInstance> {
			using Base = AbstractRenderer<MeshInstance>;
		public:
			BasicRenderer(ShaderPipeline* pipeline);
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
		};
	}
}
