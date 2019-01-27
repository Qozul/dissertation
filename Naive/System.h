/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace Naive
	{
		class System {
		public:
			System();
			~System();
			void loop();
		private:
			void initGLFW();
			void initGL3W();

			glm::mat4 viewMatrix_;

			Shared::RendererBase<BasicMesh>* basicRenderer_;
			Shared::RendererBase<TexturedBasicMesh>* texturedRenderer_;
			Shared::RendererBase<BasicMesh>* loopRenderer_;
			Shared::RendererBase<BasicMesh>* computeRenderer_;

			GLFWwindow* window_;
		};
	}
}
