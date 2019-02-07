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
			System();
			~System();
			void loop();
		private:
			void initGLFW();
			void initGL3W();

			TextureLoader* textureLoader_;
			Texture* texture_;

			glm::mat4 viewMatrix_;

			VaoWrapper* vaoWrapper_;
			AbstractRenderer<MeshInstance>* basicRenderer_;
			AbstractRenderer<TexturedMeshInstance>* texturedRenderer_;
			AbstractRenderer<MeshInstance>* loopRenderer_;
			AbstractRenderer<MeshInstance>* computeFetchRenderer_;
			AbstractRenderer<MeshInstance>* computeRenderer_;

			Shared::PerfMeasurer* basicPerfMeasurer_;
			Shared::PerfMeasurer* texturedPerfMeasurer_;
			Shared::PerfMeasurer* loopPerfMeasurer_;
			Shared::PerfMeasurer* computePerfMeasurer_;

			GLFWwindow* window_;
		};
	}
}

