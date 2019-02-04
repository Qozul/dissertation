/// Author: Ralph Ridley
/// Date: 31/12/18
#include "System.h"
#include "BasicRenderer.h"
#include "TexturedRenderer.h"
#include "LoopRenderer.h"
#include "ComputeRenderer.h"
#include "Texture.h"
#include "MeshLoader.h"
#include "Mesh.h"

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

	basicRenderer_ = new BasicRenderer(new ShaderPipeline("NaiveBasicVert", "NaiveBasicFrag"));
	basicRenderer_->addMesh(meshLoader_->loadMesh("teapot-fixed"));
	basicRenderer_->initialise();

	texture_ = new Texture("mandelbrot");
	texturedRenderer_ = new TexturedRenderer(new ShaderPipeline("NaiveTexturedVert", "NaiveTexturedFrag"));
	TexturedBasicMesh* texturedMesh = basicToTextured(meshLoader_->loadMesh("teapot-fixed"), texture_);
	texturedRenderer_->addMesh(texturedMesh->texture->id, texturedMesh);
	texturedRenderer_->initialise();

	loopRenderer_ = new LoopRenderer(new ShaderPipeline("NaiveBasicVert", "NaiveBasicFrag"));
	loopRenderer_->addMesh(meshLoader_->loadMesh("teapot-fixed"));
	loopRenderer_->initialise();

	computeRenderer_ = new ComputeRenderer(new ShaderPipeline("NaiveBasicVert", "NaiveBasicFrag"));
	computeRenderer_->addMesh(meshLoader_->loadMesh("teapot-fixed"));
	computeRenderer_->initialise();

	viewMatrix_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	QZL::Shared::checkGLError();
}

System::~System()
{
	SAFE_DELETE(basicRenderer_);
	SAFE_DELETE(texturedRenderer_);
	SAFE_DELETE(loopRenderer_);
	SAFE_DELETE(computeRenderer_); 
	SAFE_DELETE(meshLoader_);
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
		texturedRenderer_->doFrame(viewMatrix_);
		loopRenderer_->doFrame(viewMatrix_);
		computeRenderer_->doFrame(viewMatrix_);
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
