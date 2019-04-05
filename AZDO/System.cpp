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

#define NUM_OBJECTS 1000
//#define BASIC_RUN
//#define TEXTURED_RUN
#define LOOP_RUN
//#define COMPUTE_READBACK_RUN
//#define COMPUTE_RUN

using namespace QZL;
using namespace QZL::AZDO;

void errorCallback(int error, const char* description)
{
	DEBUG_ERR(description);
}

System::System()
	: basicPerfMeasurer_(new Shared::PerfMeasurer()), loopPerfMeasurer_(new Shared::PerfMeasurer()), 
	computePerfMeasurer_(new Shared::PerfMeasurer()), texturedPerfMeasurer_(new Shared::PerfMeasurer()),
	computeRbPerfMeasurer_(new Shared::PerfMeasurer())
{
	initGLFW();
	initGL3W();

	vaoWrapper_ = new VaoWrapper();
	QZL::Shared::checkGLError();
	viewMatrix_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

#ifdef BASIC_RUN
	basicRenderer_ = new BasicRenderer(new ShaderPipeline("AZDOBasicVert", "AZDOBasicFrag"), vaoWrapper_, &viewMatrix_);
	basicRenderer_->addMesh("teapot-fixed", MeshLoader::loadMesh("teapot-fixed", *vaoWrapper_));
	MeshInstance meshInst;
	for (int i = 0; i < NUM_OBJECTS; ++i) {
		basicRenderer_->addMeshInstance("teapot-fixed", &meshInst);
	}
	basicRenderer_->initialise();
#elif defined(TEXTURED_RUN)
	textureLoader_ = new TextureLoader();
	texture_ = textureLoader_->loadTexture("101");
	texture_->commit(true);
	texturedRenderer_ = new TexturedRenderer(new ShaderPipeline("AZDOTexturedVert", "AZDOTexturedFrag"), vaoWrapper_, &viewMatrix_);
	texturedRenderer_->addMesh("teapot-fixed", MeshLoader::loadMesh("teapot-fixed", *vaoWrapper_));
	TexturedMeshInstance meshInst;
	meshInst.texture = texture_;
	for (int i = 0; i < NUM_OBJECTS; ++i) {
		texturedRenderer_->addMeshInstance("teapot-fixed", &meshInst);
	}
	texturedRenderer_->initialise();
#elif defined(LOOP_RUN)
	loopRenderer_ = new LoopRenderer(new ShaderPipeline("AZDOBasicVert", "AZDOBasicFrag"), vaoWrapper_);
	loopRenderer_->addMesh("teapot-fixed", MeshLoader::loadMesh("teapot-fixed", *vaoWrapper_));
	MeshInstance meshInst;
	for (int i = 0; i < NUM_OBJECTS; ++i) {
		loopRenderer_->addMeshInstance("teapot-fixed", &meshInst);
	}
	loopRenderer_->initialise();
#elif defined(COMPUTE_READBACK_RUN)
	computeFetchRenderer_ = new ComputeFetchRenderer(new ShaderPipeline("AZDOBasicVert", "AZDOBasicFrag"), vaoWrapper_);
	computeFetchRenderer_->addMesh("teapot-fixed", MeshLoader::loadMesh("teapot-fixed", *vaoWrapper_));
	MeshInstance meshInst;
	for (int i = 0; i < NUM_OBJECTS; ++i) {
		computeFetchRenderer_->addMeshInstance("teapot-fixed", &meshInst);
	}
	computeFetchRenderer_->initialise();
#elif defined(COMPUTE_RUN)
	computeRenderer_ = new ComputeRenderer(new ShaderPipeline("AZDOBasicVert", "AZDOBasicFrag"), vaoWrapper_);
	computeRenderer_->addMesh("teapot-fixed", MeshLoader::loadMesh("teapot-fixed", *vaoWrapper_));
	MeshInstance meshInst;
	for (int i = 0; i < NUM_OBJECTS; ++i) {
		computeRenderer_->addMeshInstance("teapot-fixed", &meshInst);
	}
	computeRenderer_->initialise();
#endif

	vaoWrapper_->commit();
	QZL::Shared::checkGLError();
}

System::~System()
{
#ifdef BASIC_RUN
	SAFE_DELETE(basicRenderer_);
#elif defined(TEXTURED_RUN)
	SAFE_DELETE(texturedRenderer_);
#elif defined(LOOP_RUN)
	SAFE_DELETE(loopRenderer_);
#elif defined(COMPUTE_READBACK_RUN)
	SAFE_DELETE(computeFetchRenderer_);
#elif defined(COMPUTE_RUN)
	SAFE_DELETE(computeRenderer_);
#endif
	SAFE_DELETE(vaoWrapper_);
	SAFE_DELETE(basicPerfMeasurer_);
	SAFE_DELETE(texturedPerfMeasurer_);
	SAFE_DELETE(loopPerfMeasurer_);
	SAFE_DELETE(computePerfMeasurer_);
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

#ifdef BASIC_RUN
		basicPerfMeasurer_->startTime();
		basicRenderer_->doFrame(viewMatrix_);
		basicPerfMeasurer_->endTime();
#elif defined(TEXTURED_RUN)
		texturedPerfMeasurer_->startTime();
		texturedRenderer_->doFrame(viewMatrix_);
		texturedPerfMeasurer_->endTime();
#elif defined(LOOP_RUN)
		loopPerfMeasurer_->startTime();
		loopRenderer_->doFrame(viewMatrix_);
		loopPerfMeasurer_->endTime();
#elif defined(COMPUTE_READBACK_RUN)
		computeRbPerfMeasurer_->startTime();
		computeFetchRenderer_->doFrame(viewMatrix_);
		computeRbPerfMeasurer_->endTime();
#elif defined(COMPUTE_RUN)
		computePerfMeasurer_->startTime();
		computeRenderer_->doFrame(viewMatrix_);
		computePerfMeasurer_->endTime();
#endif
		glfwSwapBuffers(window_);
		QZL::Shared::checkGLError();
	}
#ifdef BASIC_RUN
	std::cout << "Basic perf: " << basicPerfMeasurer_->getAverageTime().count() << std::endl;
#elif defined(TEXTURED_RUN)
	std::cout << "Textured perf: " << texturedPerfMeasurer_->getAverageTime().count() << std::endl;
#elif defined(LOOP_RUN)
	std::cout << "Loop perf: " << loopPerfMeasurer_->getAverageTime().count() << std::endl;
#elif defined(COMPUTE_READBACK_RUN)
	std::cout << "Compute perf: " << computeRbPerfMeasurer_->getAverageTime().count() << std::endl;
#elif defined(COMPUTE_RUN)
	std::cout << "Compute perf: " << computePerfMeasurer_->getAverageTime().count() << std::endl;
#endif
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
