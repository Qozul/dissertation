/// Author: Ralph Ridley
/// Date: 27/01/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace Naive
	{
		class ComputeFetchRenderer : public AbstractRenderer {
			using Base = AbstractRenderer;

			struct ElementData {
				glm::mat4 model;
				glm::mat4 mvp;
			};
		public:
			ComputeFetchRenderer(ShaderPipeline* pipeline);
			~ComputeFetchRenderer();
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;

		private:
			void computeTransform(const glm::mat4& viewMatrix, BasicMesh& mesh);

			ShaderPipeline* computePipeline_;
			GLuint computeBuffer_;
			GLuint computeTransBuffer_;
			void* compTransBufPtr_;

			GLsync fence_;

			static const float kRotationSpeed;
		};
	}
}