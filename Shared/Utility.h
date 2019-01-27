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
#include <tuple>

#include <string>
#include <array>
#include <optional>
#include <functional>
#include <algorithm>

#include "PerfMeasurer.h"

// For calling delete, ensures it is not erroneous to do so
#define SAFE_DELETE(b) if (b != nullptr) { delete b; b = nullptr; }
#define CHECK_VKRESULT(result) if (result != VK_SUCCESS) throw std::runtime_error("Check VK error code " + std::to_string(static_cast<int>(result)) \
+ " in file " + __FILE__ + " at line " + std::to_string(__LINE__));
#define NOTHROW_CHECK_VKRESULT(result) if (result != VK_SUCCESS) std::cout << "Check VK error code " << std::to_string(static_cast<int>(result)) \
	<< " in file" << __FILE__ << " at line " << __LINE__ << std::endl;

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
		constexpr int kDefaultWidth = 800;
		constexpr int kDefaultHeight = 600;

		constexpr float kFoV = 45.0f;

		static const glm::mat4 kProjectionMatrix = glm::perspective(glm::radians(kFoV), 4.0f/3.0f, 0.1f, 100.0f);

		inline void checkGLError() {
			GLenum err = glGetError();
			if (err != GL_NO_ERROR)
				DEBUG_OUT("OpenGL Error " << std::hex << "0x" << err);
		}

		namespace Vulk
		{
			extern VkFormat findSupportedImageFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates,
				VkImageTiling tiling, VkFormatFeatureFlags features);

			extern uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

			extern VkCommandBuffer beginSingleTimeCommands(VkDevice logicDevice, VkCommandPool commandPool);

			extern void endSingleTimeCommands(VkDevice logicDevice, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer);

			extern void createBuffer(VkPhysicalDevice physicalDevice, VkDevice logicDevice, VkDeviceSize size, VkBufferUsageFlags usage,
				VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

			extern void copyBuffer(VkDevice logicDevice, VkCommandPool commandPool, VkQueue queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

			/// Utility function for getting data from Vulkan where it needs to call a function to get
			/// a count of the data and then call it again to get the data.
			/// Can be called succinctly with the following basic line:
			/// auto x = Shared::Vulk::obtainVkData<R>(func, arg0, arg1, ..., argN);
			template<class R, typename FP, typename... Args>
			inline std::vector<R> obtainVkData(FP func_ptr, Args&&... args)
			{
				uint32_t count = 0;
				func_ptr(std::forward<Args>(args)..., &count, nullptr);
				DEBUG_IFERR(count == 0, ("Obtain vkdata found no data for type " + std::string(typeid(R).name())));
				std::vector<R> result(count);
				func_ptr(std::forward<Args>(args)..., &count, result.data());
				return result;
			}
		}
	}
}
