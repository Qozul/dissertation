/// Author: Ralph Ridley
/// Date: 31/01/19
#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace AZDO
	{
		class ComputeRenderer : public Shared::RendererBase<BasicMesh> {
			using Base = Shared::RendererBase<BasicMesh>;
		public:
			ComputeRenderer(ShaderPipeline* pipeline);
			~ComputeRenderer();
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;

		private:
			ShaderPipeline* computePipeline_;
			GLuint computeBuffer_;
			void* compBufPtr_;
			bool compBufBound_;

			static const float kRotationSpeed;
		};
	}
}