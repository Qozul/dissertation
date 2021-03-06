/// Author: Ralph Ridley
/// Date 29/10/18
/// Purpose: Class for the Vulkan validation layer
#pragma once
#include "VkUtil.h"

#define ENABLE_VALIDATION _DEBUG

namespace QZL
{
	/// Validation will only have any effect if ENABLE_VALIDATION is true and the validation layer
	/// is supported by the system
	class Validation {
	public:
		/// Any warnings of less importance than the warning bit will be ignored
		/// Precondition: instance must have been correctly and successfully created, and is not VK_NULL_HANDLE.
		Validation(const VkInstance instance, const VkDebugUtilsMessageSeverityFlagBitsEXT severityFlag);
		~Validation();

		/// This must be called before creating a Validation instance so that the Vulkan instance has validation layers enabled
		static bool tryEnable(std::vector<const char*>& extensions, uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames);

		/// Callback function which the Vulkan validation layers call
		static VKAPI_ATTR VkBool32 VKAPI_CALL callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT type,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
	private:
		const VkInstance cInstance_;
		VkDebugUtilsMessengerEXT callbackHandle_;
		const VkDebugUtilsMessageSeverityFlagBitsEXT severityFlag_;

		static bool tryEnableSuccess;

		static const std::array<const char*, 1> kLayers;
		static const std::array<const char*, 1> kExtensions;
	};
}
