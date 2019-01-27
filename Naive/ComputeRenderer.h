/// Author: Ralph Ridley
/// Date: 27/01/18
#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace Naive
	{
		class ComputeRenderer : public Shared::RendererBase<BasicMesh> {
			using Base = Shared::RendererBase<BasicMesh>;
		public:
			ComputeRenderer(ShaderPipeline* pipeline);
			~ComputeRenderer();
			void doFrame(const glm::mat4& viewMatrix) override;

		private:
			void computeTransform(QZL::Shared::Transform& transform);

			ShaderPipeline* computePipeline_;
			GLuint computeBuffer_;
			void* compBufPtr_;

			static const float kRotationSpeed;
		};
	}
}