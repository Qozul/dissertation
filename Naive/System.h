/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace Shared
	{
		class PerfMeasurer;
	}
	namespace Naive
	{
		class AbstractRenderer;
		class TexturedRenderer;
		class MeshLoader;
		class Texture;

		class System {
		public:
			System();
			~System();
			void loop();
		private:
			void initGLFW();
			void initGL3W();

			MeshLoader* meshLoader_;

			glm::mat4 viewMatrix_;
			Texture* texture_;

			AbstractRenderer* basicRenderer_;
			TexturedRenderer* texturedRenderer_;
			AbstractRenderer* loopRenderer_;
			AbstractRenderer* computeRenderer_;
			AbstractRenderer* computeReadbackRenderer_;

			std::vector<Shared::PerfMeasurer*> perfMeasurers_;

			GLFWwindow* window_;
		};
	}
}
