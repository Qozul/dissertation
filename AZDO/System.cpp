/// Author: Ralph Ridley
/// Date: 31/01/18
#include "System.h"
#include "BasicRenderer.h"
#include "TexturedRenderer.h"
#include "LoopRenderer.h"
#include "ComputeRenderer.h"
#include "MeshLoader.h"
#include "Texture.h"
#include "TextureLoader.h"

#include "../Shared/PerfMeasurer.h"
#include "../Shared/RendererBase.h"

using namespace QZL;
using namespace QZL::AZDO;

void errorCallback(int error, const char* description)
{
	DEBUG_ERR(description);
}

System::System()
	: basicPerfMeasurer_(new Shared::PerfMeasurer()), loopPerfMeasurer_(new Shared::PerfMeasurer()), 
	computePerfMeasurer_(new Shared::PerfMeasurer()), texturedPerfMeasurer_(new Shared::PerfMeasurer())
{
	initGLFW();
	initGL3W();

	vaoWrapper_ = new VaoWrapper();
	textureLoader_ = new TextureLoader();
	texture_ = textureLoader_->loadTexture("101");
	texture_->commit(true);
	QZL::Shared::checkGLError();

	basicRenderer_ = new BasicRenderer(new ShaderPipeline("AZDOBasicVert", "AZDOBasicFrag"));
	for (int i = 0; i < 10; ++i) {
		basicRenderer_->addMesh(vaoWrapper_, "teapot-fixed", MeshLoader::loadMesh<MeshInstance>("teapot-fixed", *vaoWrapper_));
	}
	basicRenderer_->initialise();

	texturedRenderer_ = new TexturedRenderer(new ShaderPipeline("AZDOTexturedVert", "AZDOTexturedFrag"));
	for (int i = 0; i < 10; ++i) {
		auto inst = MeshLoader::loadMesh<TexturedMeshInstance>("teapot-fixed", *vaoWrapper_);
		inst->texture = texture_;
		texturedRenderer_->addMesh(vaoWrapper_, "teapot-fixed", inst);
	}
	texturedRenderer_->initialise();

	loopRenderer_ = new LoopRenderer(new ShaderPipeline("AZDOBasicVert", "AZDOBasicFrag"));
	for (int i = 0; i < 10; ++i) {
		loopRenderer_->addMesh(vaoWrapper_, "teapot-fixed", MeshLoader::loadMesh<MeshInstance>("teapot-fixed", *vaoWrapper_));
	}
	loopRenderer_->initialise();

	computeRenderer_ = new ComputeRenderer(new ShaderPipeline("AZDOBasicVert", "AZDOBasicFrag"));
	for (int i = 0; i < 10; ++i) {
		computeRenderer_->addMesh(vaoWrapper_, "teapot-fixed", MeshLoader::loadMesh<MeshInstance>("teapot-fixed", *vaoWrapper_));
	}
	computeRenderer_->initialise();

	viewMatrix_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	vaoWrapper_->commit();
	QZL::Shared::checkGLError();
}

System::~System()
{
	SAFE_DELETE(basicRenderer_);
	SAFE_DELETE(texturedRenderer_);
	SAFE_DELETE(loopRenderer_);
	SAFE_DELETE(computeRenderer_);
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

		basicPerfMeasurer_->startTime();
		basicRenderer_->doFrame(viewMatrix_);
		basicPerfMeasurer_->endTime();

		texturedPerfMeasurer_->startTime();
		texturedRenderer_->doFrame(viewMatrix_);
		texturedPerfMeasurer_->endTime();

		loopPerfMeasurer_->startTime();
		loopRenderer_->doFrame(viewMatrix_);
		loopPerfMeasurer_->endTime();

		computePerfMeasurer_->startTime();
		computeRenderer_->doFrame(viewMatrix_);
		computePerfMeasurer_->endTime();

		glfwSwapBuffers(window_);
		QZL::Shared::checkGLError();
	}
	std::cout << "Basic perf: " << basicPerfMeasurer_->getAverageTime().count() << std::endl;
	std::cout << "Textured perf: " << texturedPerfMeasurer_->getAverageTime().count() << std::endl;
	std::cout << "Loop perf: " << loopPerfMeasurer_->getAverageTime().count() << std::endl;
	std::cout << "Compute perf: " << computePerfMeasurer_->getAverageTime().count() << std::endl;
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
