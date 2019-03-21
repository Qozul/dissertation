/// Author: Ralph Ridley
/// Date: 31/01/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace AZDO
	{
		class ComputeFetchRenderer : public AbstractRenderer<MeshInstance> {
			using Base = AbstractRenderer<MeshInstance>;
		public:
			ComputeFetchRenderer(ShaderPipeline* pipeline, VaoWrapper* vao);
			~ComputeFetchRenderer();
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
			void computeTransform();

		private:
			ShaderPipeline* computePipeline_;
			GLuint computeBuffer_;
			void* compBufPtr_;

			static const float kRotationSpeed;
		};
	}
}