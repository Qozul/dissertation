/// This file can be included everywhere and defines universal includes, definitions, and functions
/// Author: Ralph Ridley
/// Date: 11/10/18
#pragma once

// Use direct vulkan.h include rather than glfw because otherwise glfw can't do both vulkan and opengl at same time
#include "gl3w.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdexcept>

#include <set>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <tuple>

#include <string>
#include <array>
#include <optional>
#include <functional>
#include <algorithm>
#include <random>

#include "PerfMeasurer.h"

// For calling delete, ensures it is not erroneous to do so
#define SAFE_DELETE(b) if (b != nullptr) { delete b; b = nullptr; }

// Debug/Release related code
#ifdef _DEBUG
#define DEBUG_OUT(m) std::cout << m << std::endl;
#define DEBUG_ERR(m) std::cerr << "Error in " << __FILE__ << " at line " << __LINE__ << " " << m << std::endl;
#define DEBUG_IFERR(b, m) if (b) DEBUG_ERR(m)
#define ENSURESM(post, msg) if (!(post)) throw std::runtime_error(msg);
#define ENSURES(post) ENSURESM(post, "Postcondition failed.")
#define EXPECTSM(pre, msg) if (!(pre)) throw std::runtime_error(msg);
#define EXPECTS(pre) EXPECTSM(pre, "Precondition failed.")
#else
#define DEBUG_OUT(m)
#define DEBUG_ERR(m)
#define DEBUG_IFERR(b, m) b;
#define ENSURES(post) post;
#define ENSURESM(post, msg) post;
#define EXPECTS(pre) pre;
#define EXPECTSM(pre, msg) pre;
#endif

namespace QZL
{
	namespace Shared
	{
		constexpr int kDefaultWidth = 800;
		constexpr int kDefaultHeight = 600;

		constexpr float kFoV = 45.0f;

		static glm::mat4 kProjectionMatrix = glm::perspective(glm::radians(kFoV), 4.0f/3.0f, 0.1f, 100.0f);

		static std::vector<std::string> kMeshNames = { "Teapot", "barrel_obj", "crate1" };
		static std::vector<std::pair<std::string, std::string>> kTextureNames = { 
			{ "101", "102" }, { "grass_01b", "grass_01_rough" }, { "ground_04", "ground_04_nrm" },
			{ "rock_05_col", "rock_05_roughness" }, { "T_crate1_D", "T_crate1_S" }
		};
		static std::random_device kRandDevice;
		static std::mt19937 kRng(kRandDevice());
		static std::uniform_int_distribution<size_t> kMeshDist(0, kMeshNames.size() - 1);
		static std::uniform_int_distribution<size_t> kTextDist(0, kTextureNames.size() - 1);

		inline void checkGLError() {
			GLenum err = glGetError();
			if (err != GL_NO_ERROR)
				DEBUG_OUT("OpenGL Error " << std::hex << "0x" << err);
		}
	}
}
