/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace Shared
	{
		class TextureLoader;
	}
	namespace Naive
	{
		class AbstractRenderer;
		class TexturedRenderer;
		class MeshLoader;

		class System {
		public:
			System();
			~System();
			void loop();
		private:
			void initGLFW();
			void initGL3W();

			MeshLoader* meshLoader_;
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
