/// Author: Ralph Ridley
/// Date: 31/01/19
#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace Shared
	{
		class TextureLoader;
	}
	namespace AZDO
	{
		template<typename InstType>
		class AbstractRenderer;
		struct MeshInstance;
		struct TexturedMeshInstance;
		class VaoWrapper;

		class System {
		public:
			System();
			~System();
			void loop();
		private:
			void initGLFW();
			void initGL3W();

			QZL::Shared::TextureLoader* textureLoader_;

			glm::mat4 viewMatrix_;

			VaoWrapper* vaoWrapper_;
			AbstractRenderer<MeshInstance>* basicRenderer_;
			AbstractRenderer<TexturedMeshInstance>* texturedRenderer_;
			AbstractRenderer<MeshInstance>* loopRenderer_;
			AbstractRenderer<MeshInstance>* computeRenderer_;

			GLFWwindow* window_;
		};
	}
}

