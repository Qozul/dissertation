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

//#define BASIC_RUN
//#define TEXTURED_RUN
//#define LOOP_RUN
//#define COMPUTE_RUN
#define COMPUTE_READBACK_RUN

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
		#define CURRENT_RENDERER computeFetchRenderer_
		#define CURRENT_RENDERER_TYPE ComputeFetchRenderer
	#endif
#endif

using namespace QZL;
using namespace QZL::AZDO;

void errorCallback(int error, const char* description)
{
	DEBUG_ERR(description);
}

System::System(int numObjsX, int numObjsY, int numObjsZ)
	: perfMeasurer_(new Shared::PerfMeasurer()), numObjsX_(numObjsX), numObjsY_(numObjsY), numObjsZ_(numObjsZ)
{
	initGLFW();
	initGL3W();

	vaoWrapper_ = new VaoWrapper();
	QZL::Shared::checkGLError();
	viewMatrix_ = glm::lookAt(glm::vec3(25.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
#ifdef TEXTURED_RUN
	textureLoader_ = new TextureLoader();
	textures_.push_back({ textureLoader_->loadTexture(Shared::kTextureNames[0].first), textureLoader_->loadTexture(Shared::kTextureNames[1].second) });
	textures_.push_back({ textureLoader_->loadTexture(Shared::kTextureNames[1].first), textureLoader_->loadTexture(Shared::kTextureNames[1].second) });
	textures_.push_back({ textureLoader_->loadTexture(Shared::kTextureNames[2].first), textureLoader_->loadTexture(Shared::kTextureNames[2].second) });
	textures_.push_back({ textureLoader_->loadTexture(Shared::kTextureNames[3].first), textureLoader_->loadTexture(Shared::kTextureNames[3].second) });
	textures_.push_back({ textureLoader_->loadTexture(Shared::kTextureNames[4].first), textureLoader_->loadTexture(Shared::kTextureNames[4].second) });
	texturedRenderer_ = new TexturedRenderer(new ShaderPipeline("AZDOTexturedVert", "AZDOTexturedFrag"), vaoWrapper_, &viewMatrix_);

	texturedRenderer_->addMesh(Shared::kMeshNames[0], MeshLoader::loadMesh(Shared::kMeshNames[0], *vaoWrapper_));
	texturedRenderer_->addMesh(Shared::kMeshNames[1], MeshLoader::loadMesh(Shared::kMeshNames[1], *vaoWrapper_));
	texturedRenderer_->addMesh(Shared::kMeshNames[2], MeshLoader::loadMesh(Shared::kMeshNames[2], *vaoWrapper_));

	TexturedMeshInstance meshInst;
	for (int i = 0; i < numObjsX_; ++i) {
		for (int j = 0; j < numObjsY_; ++j) {
			for (int k = 0; k < numObjsZ_; ++k) {
				size_t idx = Shared::kTextDist(Shared::kRng);
				auto& texPair = textures_[idx];
				meshInst.texture = texPair.first;
				meshInst.texture2 = texPair.second;

				std::string meshName = Shared::kMeshNames[Shared::kMeshDist(Shared::kRng)];
				meshInst.meshInst.transform.position = glm::vec3(i, j, k);
				meshInst.meshInst.transform.setScale(0.05f);
				texturedRenderer_->addMeshInstance(meshName, &meshInst);
			}
		}
	}
	texturedRenderer_->initialise();
#else
#ifdef BASIC_RUN
	CURRENT_RENDERER = new CURRENT_RENDERER_TYPE(new ShaderPipeline(SHADERS), vaoWrapper_, &viewMatrix_);
#else
	CURRENT_RENDERER = new CURRENT_RENDERER_TYPE(new ShaderPipeline(SHADERS), vaoWrapper_);
#endif
	CURRENT_RENDERER->addMesh(Shared::kMeshNames[0], MeshLoader::loadMesh(Shared::kMeshNames[0], *vaoWrapper_));
	CURRENT_RENDERER->addMesh(Shared::kMeshNames[1], MeshLoader::loadMesh(Shared::kMeshNames[1], *vaoWrapper_));
	CURRENT_RENDERER->addMesh(Shared::kMeshNames[2], MeshLoader::loadMesh(Shared::kMeshNames[2], *vaoWrapper_));
	MeshInstance meshInst;
	for (int i = 0; i < numObjsX_; ++i) {
		for (int j = 0; j < numObjsY_; ++j) {
			for (int k = 0; k < numObjsZ_; ++k) {
				std::string meshName = Shared::kMeshNames[Shared::kMeshDist(Shared::kRng)];
				meshInst.transform.position = glm::vec3(i, j, k);
				meshInst.transform.setScale(0.05f);
				CURRENT_RENDERER->addMeshInstance(meshName, &meshInst);
			}
		}
	}
	CURRENT_RENDERER->initialise();
#endif

	std::cout << "Drawing " << numObjsX_ * numObjsY_ * numObjsZ_ << " objects" << std::endl;
	vaoWrapper_->commit();
	QZL::Shared::checkGLError();
}

System::~System()
{
	SAFE_DELETE(CURRENT_RENDERER);
	SAFE_DELETE(vaoWrapper_);
	SAFE_DELETE(perfMeasurer_);
#ifdef TEXTURED_RUN
	for (auto texPair : textures_) {
		SAFE_DELETE(texPair.first);
		SAFE_DELETE(texPair.second);
	}
#endif
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
	glfwSwapInterval(0);
}

void System::initGL3W()
{
	if (gl3wInit()) {
		throw std::runtime_error("failed to initialize OpenGL");
	}
	DEBUG_OUT("OpenGL, GLSL\n" << glGetString(GL_VERSION) << glGetString(GL_SHADING_LANGUAGE_VERSION));
}
