/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
#include "../Shared/TestStateLoader.h"

namespace QZL
{
	namespace Naive
	{
		class AbstractRenderer;
		class TexturedRenderer;

		class System {
		public:
			System();
			~System();
			void loop();
		private:
			void initGLFW();
			void initGL3W();

			QZL::Shared::MeshLoader* meshLoader_;
			QZL::Shared::TextureLoader* textureLoader_;

			glm::mat4 viewMatrix_;

			AbstractRenderer* basicRenderer_;
			TexturedRenderer* texturedRenderer_;
			AbstractRenderer* loopRenderer_;
			AbstractRenderer* computeRenderer_;

			GLFWwindow* window_;
		};
	}
}
