/// Author: Ralph Ridley
/// Date: 31/01/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace Shared
	{
		class Transform;
	}
	namespace AZDO
	{
		class LoopRenderer : public AbstractRenderer<MeshInstance> {
			using Base = AbstractRenderer<MeshInstance>;
		public:
			LoopRenderer(ShaderPipeline* pipeline, VaoWrapper* vao);
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;

		private:
			void updateTransform(Shared::Transform& transform);

			static const float kRotationSpeed;
		};
	}
}