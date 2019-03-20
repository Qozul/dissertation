/// Author: Ralph Ridley
/// Date: 07/02/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace AZDO
	{
		class ComputeRenderer : public AbstractRenderer<MeshInstance> {
			using Base = AbstractRenderer<MeshInstance>;
		public:
			ComputeRenderer(ShaderPipeline* pipeline);
			~ComputeRenderer();
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
			void computeTransform(const glm::mat4& viewMatrix);

		private:
			ShaderPipeline* computePipeline_;
			GLuint computeBuffer_;
			void* compBufPtr_;

			static const float kRotationSpeed;
		};
	}
}
