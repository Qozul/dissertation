#pragma once
#include "VkUtil.h"

namespace QZL
{
	class Image2D;
	class LogicDevice;
	class DeviceMemory;

	class TextureLoader {
	public:
		TextureLoader(LogicDevice* logicDevice, DeviceMemory* deviceMemory);
		~TextureLoader();
		Image2D* LoadTexture(const std::string& fileName);

	private:
		std::map<std::string, Image2D*> textures_;

		LogicDevice* logicDevice_;
		DeviceMemory* deviceMemory_;

		static const std::string kPath;
		static const std::string kExt;
	};
}
