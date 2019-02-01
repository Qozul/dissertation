/// Author: Ralph Ridley
/// Date: 31/01/19
#pragma once
#include "../Shared/RendererBase.h"
#include "../Shared/TestStateLoader.h"

namespace QZL
{
	namespace AZDO
	{
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

			Shared::RendererBase<BasicMesh>* basicRenderer_;
			Shared::RendererBase<TexturedBasicMesh>* texturedRenderer_;
			Shared::RendererBase<BasicMesh>* loopRenderer_;
			Shared::RendererBase<BasicMesh>* computeRenderer_;

			GLFWwindow* window_;
		};
	}
}

