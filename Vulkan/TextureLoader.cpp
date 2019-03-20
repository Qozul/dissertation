#include "TextureLoader.h"
#include "Image2D.h"
#include "LogicDevice.h"
#include "DeviceMemory.h"
#include "../Shared/stb_image.h"

using namespace QZL;

const std::string TextureLoader::kPath = "../Assets/Textures/";
const std::string TextureLoader::kExt = ".dds";

TextureLoader::TextureLoader(LogicDevice* logicDevice, DeviceMemory* deviceMemory)
	: logicDevice_(logicDevice), deviceMemory_(deviceMemory)
{
}

TextureLoader::~TextureLoader()
{
	for (auto it : textures_) {
		SAFE_DELETE(it.second);
	}
}

// [NotMine]
Image2D* QZL::TextureLoader::LoadTexture(const std::string& fileName)
{
	DEBUG_OUT("Loading texture " << fileName);
	/*int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
		throw std::runtime_error("failed to load texture image!");
	
	MemoryAllocationDetails stagingBuffer = deviceMemory_->createBuffer(MemoryAllocationPattern::kStaging, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, imageSize);

	/*void* data; stagingBuffer.map
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);*/

	return nullptr;
}