#include "System.h"
#include "Validation.h"
#include "PhysicalDevice.h"
#include "LogicDevice.h"
#include "SwapChain.h"

using namespace QZL;

int main()
{
	System* system = new System();
	system->loop();
	delete system;
	return EXIT_SUCCESS;
}

System::System()
{
	std::vector<const char*> extensions;
	uint32_t enabledLayerCount;
	const char* const* enabledLayerNames;

	initGlfw(extensions);
	Validation::tryEnable(extensions, enabledLayerCount, enabledLayerNames);

	// Check the extensions are available
	auto availableExts = obtainVkData<VkExtensionProperties>(vkEnumerateInstanceExtensionProperties, "");
	std::vector<const char*> availableExtNames;
	std::transform(availableExts.begin(), availableExts.end(), std::back_inserter(availableExtNames),
		[](const VkExtensionProperties& prop) { return prop.extensionName; });
	for (auto ext : extensions)
		ENSURES(std::find(std::begin(availableExtNames), std::end(availableExtNames), ext) == availableExtNames.end());

	initInstance(extensions, enabledLayerCount, enabledLayerNames);
	CHECK_VKRESULT(glfwCreateWindowSurface(details_.instance, details_.window, nullptr, &details_.surface));

	validation_ = new Validation(details_.instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT);
	ENSURES(validation_ != nullptr);

	initDevices(enabledLayerCount, enabledLayerNames);
}

System::~System()
{
	SAFE_DELETE(details_.logicDevice);

	SAFE_DELETE(details_.physicalDevice);
	SAFE_DELETE(validation_);

	vkDestroySurfaceKHR(details_.instance, details_.surface, nullptr);
	vkDestroyInstance(details_.instance, nullptr);

	glfwDestroyWindow(details_.window);
	glfwTerminate();
}

void System::initGlfw(std::vector<const char*>& extensions)
{
	glfwInit();
	// Ensure glfw knows to ignore the openGL api and to not create a context for it
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	details_.window = glfwCreateWindow(kDefaultWidth, kDefaultHeight, "Vulkan Engine", nullptr, nullptr);
	
	ENSURES(details_.window != nullptr);
	
	glfwSetWindowUserPointer(details_.window, this);
	// TODO glfwSetFramebufferSizeCallback(details_.window, framebuffer_resize_callback);

	// Add the glfw extensions to the required ones
	uint32_t extensionCount = 0;
	const char** glfwExts = glfwGetRequiredInstanceExtensions(&extensionCount);

	ENSURES(glfwExts != NULL);

	for (uint32_t i = 0; i < extensionCount; i++) {
		extensions.push_back(glfwExts[i]);
	}
}

void System::initInstance(const std::vector<const char*>& extensions, uint32_t& enabledLayerCount, const char* const*& enabledLayerNames)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.flags = 0;
	createInfo.enabledLayerCount = enabledLayerCount;
	createInfo.ppEnabledLayerNames = enabledLayerNames;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	CHECK_VKRESULT(vkCreateInstance(&createInfo, nullptr, &details_.instance));
}

void System::initDevices(uint32_t& enabledLayerCount, const char* const*& enabledLayerNames)
{
	DeviceSurfaceCapabilities surfaceCapabilities;
	auto physicalDeviceHandles = obtainVkData<VkPhysicalDevice>(vkEnumeratePhysicalDevices, details_.instance);
	for (VkPhysicalDevice& handle : physicalDeviceHandles) {
		PhysicalDevice device(handle, details_.surface);
		if (device.isValid(surfaceCapabilities, details_.surface)) {
			details_.physicalDevice = new PhysicalDevice(device);
			details_.logicDevice = details_.physicalDevice->createLogicDevice(details_, surfaceCapabilities, enabledLayerCount, enabledLayerNames);
			return;
		}
	}
	ENSURES(false);
}

void System::loop()
{
	Shared::PerfMeasurer perfMeasurer;
	while (!glfwWindowShouldClose(details_.window)) {
		glfwPollEvents();
		perfMeasurer.startTime();
		details_.logicDevice->swapChain_->loop();
		perfMeasurer.endTime();
	}
	std::cout << "Vulkan perf: " << perfMeasurer.getAverageTime().count() << std::endl;
}
