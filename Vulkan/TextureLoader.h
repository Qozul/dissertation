#pragma once
#include "VkUtil.h"

namespace QZL
{
	class Image2D;
	class LogicDevice;
	class DeviceMemory;

	class TextureLoader {
	public:
		TextureLoader(const LogicDevice* logicDevice, DeviceMemory* deviceMemory);
		~TextureLoader();
		Image2D* loadTexture(const std::string& fileName);

	private:
		std::unordered_map<std::string, Image2D*> textures_;
		VkFormat convertToVkFormat(unsigned int oldFormat);

		const LogicDevice* logicDevice_;
		DeviceMemory* deviceMemory_;

		static const std::string kPath;
		static const std::string kExt;
	};
}
