/// Author: Ralph Ridley
/// Date: 27/01/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace Naive
	{
		class ComputeRenderer : public AbstractRenderer {
			using Base = AbstractRenderer;

			struct ElementData {
				glm::mat4 model;
				glm::mat4 mvp;
			};
		public:
			ComputeRenderer(ShaderPipeline* pipeline);
			~ComputeRenderer();
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;

		private:
			void computeTransform(const glm::mat4& viewMatrix, BasicMesh& mesh);

			float rot_;
			ShaderPipeline* computePipeline_;
			GLuint computeBuffer_;
			void* compBufPtr_;

			static const float kRotationSpeed;
		};
	}
}