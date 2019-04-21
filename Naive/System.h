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
			System(int numObjsX = 1, int numObjsY = 1, int numObjsZ = 1);
			~System();
			void loop();
		private:
			void initGLFW();
			void initGL3W();

			MeshLoader* meshLoader_;

			glm::mat4 viewMatrix_;
			std::vector<std::pair<Texture*, Texture*>> textures_;

			AbstractRenderer* basicRenderer_;
			TexturedRenderer* texturedRenderer_;
			AbstractRenderer* loopRenderer_;
			AbstractRenderer* computeRenderer_;
			AbstractRenderer* computeReadbackRenderer_;

			Shared::PerfMeasurer* perfMeasurer_;

			GLFWwindow* window_;

			int numObjsX_;
			int numObjsY_;
			int numObjsZ_;
		};
	}
}
