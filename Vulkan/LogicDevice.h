#pragma once
#include "VkUtil.h"

struct VmaAllocator;

namespace QZL
{
	class SwapChain;
	struct SystemDetails;

	enum class QueueFamilyType : size_t {
		kGraphicsQueue = 0,
		kPresentationQueue,
		kNumQueueFamilyTypes // Do not index with this, this is the size
	};

	struct DeviceSurfaceCapabilities {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class LogicDevice {
		friend class PhysicalDevice;
		friend class System;
	public:
		VkDevice getLogicDevice() const;

		const uint32_t getFamilyIndex(QueueFamilyType type) const; // needed in swap chain
		const std::vector<uint32_t>& getAllIndices() const noexcept; // needed in swap chain
		VkQueue getQueueHandle(QueueFamilyType type);

	private:
		LogicDevice(VkDevice device, const SystemDetails& sysDetails, DeviceSurfaceCapabilities& surfaceCapabilities,
			std::vector<uint32_t> indices, std::vector<VkQueue> handles);
		~LogicDevice();

		VkDevice device_;
		VkCommandPool primaryCommandPool_;
		VmaAllocator* allocator_;

		SwapChain* swapChain_;

		std::vector<uint32_t> queueFamilyIndices_;
		std::vector<VkQueue> queueHandles_;
	};
}
