/// Author: Ralph Ridley
/// Date: 31/12/18
#include "System.h"
#include "BasicRenderer.h"
#include "TexturedRenderer.h"
#include "LoopRenderer.h"
#include "ComputeRenderer.h"
#include "ComputeFetchRenderer.h"
#include "Texture.h"
#include "MeshLoader.h"
#include "Mesh.h"
#include "../Shared/PerfMeasurer.h"

#define NUM_OBJECTS_X 10
#define NUM_OBJECTS_Y 10
#define NUM_OBJECTS_Z 10
#define BASIC_RUN
//#define TEXTURED_RUN
//#define LOOP_RUN
//#define COMPUTE_RUN
//#define COMPUTE_READBACK_RUN

#ifdef TEXTURED_RUN
	#define CURRENT_RENDERER texturedRenderer_
	#define CURRENT_RENDERER_TYPE TexturedRenderer
	#define SHADERS "NaiveTexturedVert", "NaiveTexturedFrag"
#else
	#define SHADERS "NaiveBasicVert", "NaiveBasicFrag"
	#ifdef BASIC_RUN
		#define CURRENT_RENDERER basicRenderer_
		#define CURRENT_RENDERER_TYPE BasicRenderer
	#elif defined(LOOP_RUN)
		#define CURRENT_RENDERER loopRenderer_
		#define CURRENT_RENDERER_TYPE LoopRenderer
	#elif defined(COMPUTE_RUN)
		#define CURRENT_RENDERER computeRenderer_
		#define CURRENT_RENDERER_TYPE ComputeRenderer
		#define SHADERS "NaiveComputeVert", "NaiveComputeFrag"
	#elif defined(COMPUTE_READBACK_RUN)
		#define CURRENT_RENDERER computeReadbackRenderer_
		#define CURRENT_RENDERER_TYPE ComputeFetchRenderer
		#define SHADERS "NaiveComputeVert", "NaiveComputeFrag"
	#endif
#endif

using namespace QZL;
using namespace QZL::Naive;

void errorCallback(int error, const char* description)
{
	DEBUG_ERR(description);
}

System::System()
	: meshLoader_(new MeshLoader()), perfMeasurer_(new Shared::PerfMeasurer())
{
	initGLFW();
	initGL3W();

	viewMatrix_ = glm::lookAt(glm::vec3(25.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


#ifdef TEXTURED_RUN
	texture_ = new Texture("mandelbrot");
	texturedRenderer_ = new TexturedRenderer(new ShaderPipeline(SHADERS));
	for (int i = 0; i < NUM_OBJECTS_X; ++i) {
		for (int j = 0; j < NUM_OBJECTS_Y; ++j) {
			for (int k = 0; k < NUM_OBJECTS_Z; ++k) {
				TexturedBasicMesh* texturedMesh = basicToTextured(meshLoader_->loadMesh("teapot-fixed"), texture_);
				mesh->transform.position = glm::vec3(i, j, k);
				mesh->transform.setScale(0.2f);
				texturedRenderer_->addMesh(texturedMesh->texture->id, texturedMesh);
			}
		}
	}
	texturedRenderer_->initialise();
#else
#ifdef BASIC_RUN
	CURRENT_RENDERER = new CURRENT_RENDERER_TYPE(new ShaderPipeline(SHADERS), &viewMatrix_);
#else
	CURRENT_RENDERER = new CURRENT_RENDERER_TYPE(new ShaderPipeline(SHADERS));
#endif

	for (int i = 0; i < NUM_OBJECTS_X; ++i) {
		for (int j = 0; j < NUM_OBJECTS_Y; ++j) {
			for (int k = 0; k < NUM_OBJECTS_Z; ++k) {
				auto mesh = meshLoader_->loadMesh("teapot-fixed");
				mesh->transform.position = glm::vec3(i, j, k);
				mesh->transform.setScale(0.2f);
				CURRENT_RENDERER->addMesh(mesh);
			}
		}
	}
	CURRENT_RENDERER->initialise();
	std::cout << "Drawing " << NUM_OBJECTS_X * NUM_OBJECTS_Y * NUM_OBJECTS_Z << " objects" << std::endl;
#endif
	QZL::Shared::checkGLError();
}

System::~System()
{
	SAFE_DELETE(CURRENT_RENDERER);
	SAFE_DELETE(meshLoader_);
	SAFE_DELETE(perfMeasurer_);
	glfwDestroyWindow(window_);
	glfwTerminate();
}

void System::loop()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window_)) {
		glfwPollEvents();
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		perfMeasurer_->startTime();
		CURRENT_RENDERER->doFrame(viewMatrix_);
		perfMeasurer_->endTime();

		glfwSwapBuffers(window_);

		//QZL::Shared::checkGLError();
	}

	std::cout << "Naive Frame Time: " << perfMeasurer_->getAverageTime().count() << std::endl;
}

void System::initGLFW()
{
	glfwInit();
	window_ = glfwCreateWindow(Shared::kDefaultWidth, Shared::kDefaultHeight, "OpenGL Engine: Naive", nullptr, nullptr);
	ENSURES(window_ != nullptr);
	//glfwSetWindowUserPointer(window_, this);
	glfwSetErrorCallback(errorCallback);
	glfwMakeContextCurrent(window_);
}

void System::initGL3W()
{
	if (gl3wInit()) {
		throw std::runtime_error("failed to initialize OpenGL");
	}
	DEBUG_OUT("OpenGL, GLSL\n" << glGetString(GL_VERSION) << glGetString(GL_SHADING_LANGUAGE_VERSION));
}
