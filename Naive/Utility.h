/// This file can be included everywhere and defines universal includes, definitions, and functions
/// Author: Ralph Ridley
/// Date: 11/10/18
#pragma once

//#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>

#include <set>
#include <vector>
#include <map>
#include <tuple>

#include <string>
#include <array>
#include <optional>
#include <functional>
#include <algorithm>

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
#define DEBUG_IF_ERR(b, m)
#define ENSURES(pre, msg)
#define EXPECTS(post, msg)
#endif

namespace QZL
{
	namespace Shared
	{
	}
}
