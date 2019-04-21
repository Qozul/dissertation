/// Author: Ralph Ridley
/// Date: 31/01/19
#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace Shared
	{
		class PerfMeasurer;
	}
	namespace AZDO
	{
		template<typename InstType>
		class AbstractRenderer;
		struct MeshInstance;
		struct TexturedMeshInstance;
		class VaoWrapper;
		class Texture;
		class TextureLoader;

		class System {
		public:
			System(int numObjsX = 1, int numObjsY = 1, int numObjsZ = 1);
			~System();
			void loop();
		private:
			void initGLFW();
			void initGL3W();

			TextureLoader* textureLoader_;
			std::vector<std::pair<Texture*, Texture*>> textures_;

			glm::mat4 viewMatrix_;

			VaoWrapper* vaoWrapper_;
			AbstractRenderer<MeshInstance>* basicRenderer_;
			AbstractRenderer<TexturedMeshInstance>* texturedRenderer_;
			AbstractRenderer<MeshInstance>* loopRenderer_;
			AbstractRenderer<MeshInstance>* computeFetchRenderer_;
			AbstractRenderer<MeshInstance>* computeRenderer_;
			Shared::PerfMeasurer* perfMeasurer_;

			GLFWwindow* window_;

			int numObjsX_;
			int numObjsY_;
			int numObjsZ_;
		};
	}
}

