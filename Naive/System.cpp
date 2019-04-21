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

//#define BASIC_RUN
//#define TEXTURED_RUN
//#define LOOP_RUN
#define COMPUTE_RUN
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

System::System(int numObjsX, int numObjsY, int numObjsZ)
	: meshLoader_(new MeshLoader()), perfMeasurer_(new Shared::PerfMeasurer()), numObjsX_(numObjsX), numObjsY_(numObjsY), numObjsZ_(numObjsZ)
{
	initGLFW();
	initGL3W();

	viewMatrix_ = glm::lookAt(glm::vec3(25.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

#ifdef TEXTURED_RUN
	textures_.push_back({ new Texture(Shared::kTextureNames[0].first), new Texture(Shared::kTextureNames[0].second) });
	textures_.push_back({ new Texture(Shared::kTextureNames[1].first), new Texture(Shared::kTextureNames[1].second) });
	textures_.push_back({ new Texture(Shared::kTextureNames[2].first), new Texture(Shared::kTextureNames[2].second) });
	textures_.push_back({ new Texture(Shared::kTextureNames[3].first), new Texture(Shared::kTextureNames[3].second) });
	textures_.push_back({ new Texture(Shared::kTextureNames[4].first), new Texture(Shared::kTextureNames[4].second) });
	texturedRenderer_ = new TexturedRenderer(new ShaderPipeline(SHADERS));
	for (int i = 0; i < numObjsX_; ++i) {
		for (int j = 0; j < numObjsY_; ++j) {
			for (int k = 0; k < numObjsZ_; ++k) {
				size_t idx = Shared::kTextDist(Shared::kRng);
				auto& texPair = textures_[idx];
				TexturedBasicMesh* texturedMesh = basicToTextured(meshLoader_->loadMesh(Shared::kMeshNames[Shared::kMeshDist(Shared::kRng)]), 
					texPair.first, texPair.second);
				texturedMesh->transform.position = glm::vec3(i, j, k);
				texturedMesh->transform.setScale(0.05f);
				texturedRenderer_->addMesh(idx, texturedMesh);
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

	for (int i = 0; i < numObjsX_; ++i) {
		for (int j = 0; j < numObjsY_; ++j) {
			for (int k = 0; k < numObjsZ_; ++k) {
				auto mesh = meshLoader_->loadMesh(Shared::kMeshNames[Shared::kMeshDist(Shared::kRng)]);
				mesh->transform.position = glm::vec3(i, j, k);
				mesh->transform.setScale(0.05f);
				CURRENT_RENDERER->addMesh(mesh);
			}
		}
	}
	CURRENT_RENDERER->initialise();
	std::cout << "Drawing " << numObjsX_ * numObjsY_ * numObjsZ_ << " objects" << std::endl;
#endif
	QZL::Shared::checkGLError();
}

System::~System()
{
	SAFE_DELETE(CURRENT_RENDERER);
	SAFE_DELETE(meshLoader_);
	SAFE_DELETE(perfMeasurer_);
#ifdef TEXTURED_RUN
	for (auto texPair : textures_) {
		SAFE_DELETE(texPair.first);
		SAFE_DELETE(texPair.second);
	}
#endif
	glfwDestroyWindow(window_);
	glfwTerminate();
}

void System::loop()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window_)) {
		glfwPollEvents();
		perfMeasurer_->startTime();
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CURRENT_RENDERER->doFrame(viewMatrix_);

		glfwSwapBuffers(window_);
		perfMeasurer_->endTime();

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
	glfwSwapInterval(0);
}

void System::initGL3W()
{
	if (gl3wInit()) {
		throw std::runtime_error("failed to initialize OpenGL");
	}
	DEBUG_OUT("OpenGL, GLSL\n" << glGetString(GL_VERSION) << glGetString(GL_SHADING_LANGUAGE_VERSION));
}
