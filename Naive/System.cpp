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

#define NUM_OBJECTS 10
//#define BASIC_RUN
//#define TEXTURED_RUN
//#define LOOP_RUN
#define COMPUTE_READBACK_RUN
//#define COMPUTE_RUN

using namespace QZL;
using namespace QZL::Naive;

void errorCallback(int error, const char* description)
{
	DEBUG_ERR(description);
}

System::System()
	: meshLoader_(new MeshLoader())
{
	initGLFW();
	initGL3W();

	for (int i = 0; i < 5; ++i)
		perfMeasurers_.push_back(new Shared::PerfMeasurer());

	viewMatrix_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

#ifdef BASIC_RUN
	basicRenderer_ = new BasicRenderer(new ShaderPipeline("NaiveBasicVert", "NaiveBasicFrag"), &viewMatrix_);
	for (int i = 0; i < NUM_OBJECTS; ++i)
		basicRenderer_->addMesh(meshLoader_->loadMesh("teapot-fixed"));
	basicRenderer_->initialise();
#elif defined(TEXTURED_RUN)
	texture_ = new Texture("mandelbrot");
	texturedRenderer_ = new TexturedRenderer(new ShaderPipeline("NaiveTexturedVert", "NaiveTexturedFrag"));
	for (int i = 0; i < NUM_OBJECTS; ++i) {
		TexturedBasicMesh* texturedMesh = basicToTextured(meshLoader_->loadMesh("teapot-fixed"), texture_);
		texturedRenderer_->addMesh(texturedMesh->texture->id, texturedMesh);
	}
	texturedRenderer_->initialise();
#elif defined(LOOP_RUN)
	loopRenderer_ = new LoopRenderer(new ShaderPipeline("NaiveBasicVert", "NaiveBasicFrag"));
	for (int i = 0; i < NUM_OBJECTS; ++i)
		loopRenderer_->addMesh(meshLoader_->loadMesh("teapot-fixed"));
	loopRenderer_->initialise();
#elif defined(COMPUTE_READBACK_RUN)
	computeReadbackRenderer_ = new ComputeFetchRenderer(new ShaderPipeline("NaiveComputeVert", "NaiveComputeFrag"));
	for (int i = 0; i < NUM_OBJECTS; ++i)
		computeReadbackRenderer_->addMesh(meshLoader_->loadMesh("teapot-fixed"));
	computeReadbackRenderer_->initialise();
#elif defined(COMPUTE_RUN)
	computeRenderer_ = new ComputeRenderer(new ShaderPipeline("NaiveComputeVert", "NaiveComputeFrag"));
	for (int i = 0; i < NUM_OBJECTS; ++i)
		computeRenderer_->addMesh(meshLoader_->loadMesh("teapot-fixed"));
	computeRenderer_->initialise();
#endif
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
	SAFE_DELETE(computeReadbackRenderer_);
#elif defined(COMPUTE_RUN)
	SAFE_DELETE(computeRenderer_);
#endif
	SAFE_DELETE(meshLoader_);
	for (auto& perfMeasurer : perfMeasurers_) {
		SAFE_DELETE(perfMeasurer);
	}
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
		perfMeasurers_[0]->startTime();
		basicRenderer_->doFrame(viewMatrix_);
		perfMeasurers_[0]->endTime();
#elif defined(TEXTURED_RUN)
		perfMeasurers_[1]->startTime();
		texturedRenderer_->doFrame(viewMatrix_);
		perfMeasurers_[1]->endTime();
#elif defined(LOOP_RUN)
		perfMeasurers_[2]->startTime();
		loopRenderer_->doFrame(viewMatrix_);
		perfMeasurers_[2]->endTime();
#elif defined(COMPUTE_READBACK_RUN)
		perfMeasurers_[3]->startTime();
		computeReadbackRenderer_->doFrame(viewMatrix_);
		perfMeasurers_[3]->endTime();
#elif defined(COMPUTE_RUN)
		perfMeasurers_[4]->startTime();
		computeRenderer_->doFrame(viewMatrix_);
		perfMeasurers_[4]->endTime();
#endif
		glfwSwapBuffers(window_);

		QZL::Shared::checkGLError();
	}

#ifdef BASIC_RUN
	std::cout << "Basic perf: " << perfMeasurers_[0]->getAverageTime().count() << std::endl;
#elif defined(TEXTURED_RUN)
	std::cout << "Textured perf: " << perfMeasurers_[1]->getAverageTime().count() << std::endl;
#elif defined(LOOP_RUN)
	std::cout << "Loop perf: " << perfMeasurers_[2]->getAverageTime().count() << std::endl;
#elif defined(COMPUTE_READBACK_RUN)
	std::cout << "Compute Fetch perf: " << perfMeasurers_[3]->getAverageTime().count() << std::endl;
#elif defined(COMPUTE_RUN)
	std::cout << "Compute perf: " << perfMeasurers_[4]->getAverageTime().count() << std::endl;
#endif
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
