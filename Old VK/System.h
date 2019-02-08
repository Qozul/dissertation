/// Author: Ralph Ridley
/// Date 29/10/18
/// Purpose: Class to define the system environment of the engine
#pragma once
#include "../Shared/Utility.h"
#include "SwapChain.h"

namespace QZL
{
	class Validation;
	class QueueFamilies;
	class Device;

	struct SystemDetails {
		GLFWwindow* window = nullptr;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice logicDevice = VK_NULL_HANDLE;
		VkCommandPool primaryCommandPool = VK_NULL_HANDLE;
	};

	class System {
	public:
		System();
		~System();

		void loop();

		/// Supreme constness, no changing core details if you aren't System
		const SystemDetails* const getSysDetails() const noexcept;
	private:
		VkInstance setupCoreSystem(uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames);
		void initGLFW(std::vector<const char*>& extensions);
		void requiredExtensionsAvailable(const std::vector<const char*>& extensions);
		void createVkInstance(const std::vector<const char*>& extensions, uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames);
		void createPhysicalDevice();
		void createLogicDevice(uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames);

		SystemDetails details_;
		Validation* validation_;
		Device* device_;
		SwapChain* swapChain_;
		DeviceSwapChainDetails swapChainDetails_;

		static const int kDefaultWidth = 800;
		static const int kDefaultHeight = 600;
	};
}
