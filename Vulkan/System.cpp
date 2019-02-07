/// Author: Ralph Ridley
/// Date 29/10/18
/// Purpose: Definitions for System.h
#include "System.h"
#include "Validation.h"
#include "Device.h"

using namespace QZL;

System::System()
	: validation_(nullptr), device_(nullptr), swapChain_(nullptr)
{
	uint32_t enabledLayerCount;
	const char* const* ppEnabledLayerNames;
	setupCoreSystem(enabledLayerCount, ppEnabledLayerNames);
	validation_ = new Validation(details_.instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT);
	ENSURES(validation_ != nullptr);
	createPhysicalDevice();
	createLogicDevice(enabledLayerCount, ppEnabledLayerNames);
	ENSURES(device_ != nullptr);
	details_.primaryCommandPool = device_->createCommandPool(); // TODO move to swapchain as owner
	swapChain_ = new SwapChain(device_, details_.surface, details_.window, swapChainDetails_, details_.primaryCommandPool);
	ENSURES(swapChain_ != nullptr);

	// SwapChain ->(swapChain, imageViews, depth, descriptor pool and sets?, framebuffers?, Synchronisation?)
	// RenderPass ->(renderPass, descriptor sets, GfxPipelines, command pool, command buffers)

	// Where for vertex, index, and uniform buffers? RenderPass? Outside RenderPass?

	ENSURES(details_.window != nullptr && details_.instance != VK_NULL_HANDLE && details_.physicalDevice != VK_NULL_HANDLE
		&& details_.logicDevice != VK_NULL_HANDLE && details_.surface != VK_NULL_HANDLE);
}

System::~System()
{
	vkDeviceWaitIdle(details_.logicDevice);
	SAFE_DELETE(swapChain_);
	// After deleting the device the logicDevice and commandPool become invalid
	SAFE_DELETE(device_);
	details_.primaryCommandPool = VK_NULL_HANDLE;
	details_.logicDevice = VK_NULL_HANDLE;

	SAFE_DELETE(validation_);
	vkDestroySurfaceKHR(details_.instance, details_.surface, nullptr);
	vkDestroyInstance(details_.instance, nullptr);
	glfwDestroyWindow(details_.window);
	glfwTerminate();
}

const SystemDetails* const System::getSysDetails() const noexcept
{
	return &details_;
}

void System::loop()
{
	while (!glfwWindowShouldClose(details_.window)) {
		glfwPollEvents();
		swapChain_->loop();
	}

	vkDeviceWaitIdle(details_.logicDevice);
}

VkInstance System::setupCoreSystem(uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames)
{
	std::vector<const char*> extensions;
	// GLFWwindow*
	initGLFW(extensions);
	// Validation
	Validation::tryEnable(extensions, enabledLayerCount, ppEnabledLayerNames);
	// Must not change extensions after this function below
	requiredExtensionsAvailable(extensions);
	// VkInstance
	createVkInstance(extensions, enabledLayerCount, ppEnabledLayerNames);
	// VkSurfaceKHR
	CHECK_VKRESULT(glfwCreateWindowSurface(details_.instance, details_.window, nullptr, &details_.surface));
	// Return VkInstance for constructing a validation instance in the System initialiser list
	return details_.instance;
}

void System::initGLFW(std::vector<const char*>& extensions)
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

void System::requiredExtensionsAvailable(const std::vector<const char*>& extensions)
{
	auto availableExtensions = Shared::Vulk::obtainVkData<VkExtensionProperties>(vkEnumerateInstanceExtensionProperties, "");
	bool found = false;
	for (auto ext : extensions) {
		for (const auto& extension : availableExtensions) {
			if (strcmp(ext, extension.extensionName) == 0) {
				found = true;
				break;
			}
		}
		if (!found)
			throw std::runtime_error("Missing required extension " + std::string(ext));
	}
}

void System::createVkInstance(const std::vector<const char*>& extensions, uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.flags = 0;
	createInfo.enabledLayerCount = enabledLayerCount;
	createInfo.ppEnabledLayerNames = ppEnabledLayerNames;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	CHECK_VKRESULT(vkCreateInstance(&createInfo, nullptr, &details_.instance));
}

void System::createPhysicalDevice()
{
	auto physicalDevices = Shared::Vulk::obtainVkData<VkPhysicalDevice>(vkEnumeratePhysicalDevices, details_.instance);
	for (VkPhysicalDevice& physicalDevice : physicalDevices) {
		Device* device = new Device(physicalDevice, details_.surface);
		DeviceSwapChainDetails swapChainDetails;
		if (device->hasRequiredQueueFamilies() && device->isSuitable(&swapChainDetails, details_.surface)) {
			details_.physicalDevice = physicalDevice;
			device_ = device;
			swapChainDetails_ = swapChainDetails;
			break;
		}
	}
	ENSURES(details_.physicalDevice != VK_NULL_HANDLE);
}

void System::createLogicDevice(uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames)
{
	// Once the physical device has been selected, the logic device can be created
	details_.logicDevice = device_->createLogicDevice(enabledLayerCount, ppEnabledLayerNames);
}
