/// Author: Ralph Ridley
/// Date: 31/01/18
#include "System.h"
#include "BasicRenderer.h"
#include "TexturedRenderer.h"
#include "LoopRenderer.h"
#include "ComputeRenderer.h"
#include "MeshLoader.h"
#include "../Shared/TextureLoader.h"

using namespace QZL;
using namespace QZL::AZDO;

void errorCallback(int error, const char* description)
{
	DEBUG_ERR(description);
}

System::System()
	: textureLoader_(new QZL::Shared::TextureLoader())
{
	initGLFW();
	initGL3W();

	vaoWrapper_ = new VaoWrapper();

	basicRenderer_ = new BasicRenderer(new ShaderPipeline("AZDOBasicVert", "AZDOBasicFrag"));
	
	basicRenderer_->addMesh(vaoWrapper_, "teapot-fixed", MeshLoader::loadMesh<MeshInstance>("teapot-fixed", *vaoWrapper_));
	basicRenderer_->initialise();

	/*texturedRenderer_ = new TexturedRenderer(new ShaderPipeline("NaiveTexturedVert", "NaiveTexturedFrag"));
	Naive::TexturedBasicMesh* textured = basicToTextured(meshLoader_->loadNaiveMesh("teapot-fixed"),
		textureLoader_->loadNaiveTexture("Mandelbrot"));
	texturedRenderer_->addMesh(textured->texture->id, textured);
	texturedRenderer_->initialise();

	loopRenderer_ = new LoopRenderer(new ShaderPipeline("NaiveBasicVert", "NaiveBasicFrag"));
	loopRenderer_->addMesh(0, meshLoader_->loadNaiveMesh("teapot-fixed"));
	loopRenderer_->initialise();

	computeRenderer_ = new ComputeRenderer(new ShaderPipeline("NaiveBasicVert", "NaiveBasicFrag"));
	computeRenderer_->addMesh(0, meshLoader_->loadNaiveMesh("teapot-fixed"));
	computeRenderer_->initialise();*/

	viewMatrix_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	vaoWrapper_->commit();
	QZL::Shared::checkGLError();
}

System::~System()
{
	SAFE_DELETE(basicRenderer_);
	/*SAFE_DELETE(texturedRenderer_);
	SAFE_DELETE(loopRenderer_);
	SAFE_DELETE(computeRenderer_);*/
	SAFE_DELETE(textureLoader_);
	SAFE_DELETE(vaoWrapper_);
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
		basicRenderer_->doFrame(viewMatrix_);
		/*texturedRenderer_->doFrame(viewMatrix_);
		loopRenderer_->doFrame(viewMatrix_);
		computeRenderer_->doFrame(viewMatrix_);*/
		glfwSwapBuffers(window_);

		QZL::Shared::checkGLError();
	}
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
