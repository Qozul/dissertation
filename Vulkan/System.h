#pragma once
#include "VkUtil.h"

int main();

namespace QZL
{
	struct DeviceSwapChainDetails;
	class PhysicalDevice;
	class LogicDevice;
	class CommandPool;

	class Validation;

	struct SystemDetails {
		GLFWwindow* window = nullptr;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkInstance instance = VK_NULL_HANDLE;

		PhysicalDevice* physicalDevice = nullptr;
		LogicDevice* logicDevice = nullptr;
	};

	class System {
		friend int ::main();
	private:
		System();
		~System();

		void initGlfw(std::vector<const char*>& extensions);
		void initInstance(const std::vector<const char*>& extensions, uint32_t& enabledLayerCount, 
			const char* const*& enabledLayerNames);
		void initDevices(uint32_t& enabledLayerCount, const char* const*& enabledLayerNames);

		void loop();

		SystemDetails details_;
		Validation* validation_;

		static const int kDefaultWidth = 800;
		static const int kDefaultHeight = 600;
	};
}
