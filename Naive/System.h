/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
#include "RendererBase.h"

namespace QZL
{
	class System {
	public:
		System();
		~System();
		void loop();
	private:
		void initGLFW();
		void initGL3W();

		RendererBase* basicRenderer_;

		GLFWwindow* window_;

		static const int kDefaultWidth = 800;
		static const int kDefaultHeight = 600;
	};
}
