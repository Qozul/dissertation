/// Author: Ralph Ridley
/// Date: 31/01/18
#include "System.h"
#include "BasicRenderer.h"
#include "TexturedRenderer.h"
#include "LoopRenderer.h"
#include "ComputeFetchRenderer.h"
#include "ComputeRenderer.h"
#include "MeshLoader.h"
#include "Texture.h"
#include "TextureLoader.h"

#include "../Shared/PerfMeasurer.h"
#include "../Shared/RendererBase.h"

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
	#define SHADERS "AZDOTexturedVert", "AZDOTexturedFrag"
#else
	#define SHADERS "AZDOBasicVert", "AZDOBasicFrag"
	#ifdef BASIC_RUN
		#define CURRENT_RENDERER basicRenderer_
		#define CURRENT_RENDERER_TYPE BasicRenderer
	#elif defined(LOOP_RUN)
		#define CURRENT_RENDERER loopRenderer_
		#define CURRENT_RENDERER_TYPE LoopRenderer
	#elif defined(COMPUTE_RUN)
		#define CURRENT_RENDERER computeRenderer_
		#define CURRENT_RENDERER_TYPE ComputeRenderer
	#elif defined(COMPUTE_READBACK_RUN)
		#define CURRENT_RENDERER computeReadbackRenderer_
		#define CURRENT_RENDERER_TYPE ComputeFetchRenderer
	#endif
#endif

using namespace QZL;
using namespace QZL::AZDO;

void errorCallback(int error, const char* description)
{
	DEBUG_ERR(description);
}

System::System()
	: perfMeasurer_(new Shared::PerfMeasurer())
{
	initGLFW();
	initGL3W();

	vaoWrapper_ = new VaoWrapper();
	QZL::Shared::checkGLError();
	viewMatrix_ = glm::lookAt(glm::vec3(25.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
#ifdef TEXTURED_RUN
	textureLoader_ = new TextureLoader();
	texture_ = textureLoader_->loadTexture("101");
	texture_->commit(true);
	texturedRenderer_ = new TexturedRenderer(new ShaderPipeline("AZDOTexturedVert", "AZDOTexturedFrag"), vaoWrapper_, &viewMatrix_);
	texturedRenderer_->addMesh("teapot-fixed", MeshLoader::loadMesh("teapot-fixed", *vaoWrapper_));
	TexturedMeshInstance meshInst;
	meshInst.texture = texture_;
	for (int i = 0; i < NUM_OBJECTS_X; ++i) {
		for (int j = 0; j < NUM_OBJECTS_Y; ++j) {
			for (int k = 0; k < NUM_OBJECTS_Z; ++k) {
				meshInst.transform.position = glm::vec3(i, j, k);
				meshInst.transform.setScale(0.2f);
				texturedRenderer_->addMeshInstance("teapot-fixed", &meshInst);
			}
		}
	}
	texturedRenderer_->initialise();
#else
	CURRENT_RENDERER = new CURRENT_RENDERER_TYPE(new ShaderPipeline(SHADERS), vaoWrapper_, &viewMatrix_);
	CURRENT_RENDERER->addMesh("teapot-fixed", MeshLoader::loadMesh("teapot-fixed", *vaoWrapper_));
	MeshInstance meshInst;
	for (int i = 0; i < NUM_OBJECTS_X; ++i) {
		for (int j = 0; j < NUM_OBJECTS_Y; ++j) {
			for (int k = 0; k < NUM_OBJECTS_Z; ++k) {
				meshInst.transform.position = glm::vec3(i, j, k);
				meshInst.transform.setScale(0.2f);
				CURRENT_RENDERER->addMeshInstance("teapot-fixed", &meshInst);
			}
		}
	}
	CURRENT_RENDERER->initialise();
#endif

	std::cout << "Drawing " << NUM_OBJECTS_X * NUM_OBJECTS_Y* NUM_OBJECTS_Z << " objects" << std::endl;
	vaoWrapper_->commit();
	QZL::Shared::checkGLError();
}

System::~System()
{
	SAFE_DELETE(CURRENT_RENDERER);
	SAFE_DELETE(vaoWrapper_);
	SAFE_DELETE(perfMeasurer_);
	SAFE_DELETE(texture_);
	SAFE_DELETE(textureLoader_);
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
		QZL::Shared::checkGLError();
	}
	std::cout << "AZDO Frame Time: " << perfMeasurer_->getAverageTime().count() << std::endl;
}

void System::initGLFW()
{
	glfwInit();
	window_ = glfwCreateWindow(Shared::kDefaultWidth, Shared::kDefaultHeight, "OpenGL Engine: AZDO", nullptr, nullptr);
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
