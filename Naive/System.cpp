/// Author: Ralph Ridley
/// Date: 31/12/18
#include "System.h"
//#include "Utility.h"
#include "../Shared/Utility.h"
#include "NaiveBasicRenderer.h"

// Uncomment the define below depending on Naive/AZDO usage of the engine
//#define USE_AZDO
#ifdef USE_AZDO
using namespace QZL::AZDO;
#else
using namespace QZL::Naive;
#endif

using namespace QZL;

void errorCallback(int error, const char* description)
{
	DEBUG_ERR(description);
}

System::System()
{
	initGLFW();
	initGL3W();
	//basicRenderer_ = new BasicRenderer();
	basicRenderer_ = nullptr;
}

System::~System()
{
	SAFE_DELETE(basicRenderer_);
	glfwDestroyWindow(window_);
	glfwTerminate();
}

void System::loop()
{
	while (!glfwWindowShouldClose(window_)) {
		glfwPollEvents();
		basicRenderer_->doFrame();
	}
}

void System::initGLFW()
{
	glfwInit();

	window_ = glfwCreateWindow(kDefaultWidth, kDefaultHeight, "OpenGL Engine", nullptr, nullptr);
	ENSURES(window_ != nullptr);
	//glfwSetWindowUserPointer(window_, this);
	glfwSetErrorCallback(errorCallback);
}

void System::initGL3W()
{
	if (gl3wInit()) {
		throw std::runtime_error("failed to initialize OpenGL");
	}
	if (!gl3wIsSupported(3, 2)) {
		throw std::runtime_error("OpenGL 3.2 not supported");
	}
	DEBUG_OUT("OpenGL %s, GLSL %s\n" << glGetString(GL_VERSION) << glGetString(GL_SHADING_LANGUAGE_VERSION));
}
