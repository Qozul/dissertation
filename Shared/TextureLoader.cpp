#include "TextureLoader.h"

using namespace QZL;
using namespace QZL::Shared;

const std::string TextureLoader::kPath = "../Assets/Textures/";
const std::string TextureLoader::kExt = ".dds";

Naive::Texture* TextureLoader::loadNaiveTexture(const std::string& fileName)
{
	DEBUG_OUT("Loading texture " << fileName);
	auto texIterator = naiveTextures_.find(fileName);
	Naive::Texture* texture;
	if (texIterator != naiveTextures_.end()) {
		DEBUG_OUT("Finished loading " << fileName);
		return texIterator->second;
	}
	else {
		texture = new Naive::Texture();
		glGenTextures(1, &texture->id);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}

	auto imgIterator = nvddsImages_.find(fileName);
	nv_dds::CDDSImage* image;
	if (imgIterator != nvddsImages_.end()) {
		image = imgIterator->second;
	}
	else {
		image = new nv_dds::CDDSImage();
		image->load(kPath + fileName + kExt);
		nvddsImages_[fileName] = image;
	}
	image->upload_texture2D();
	glBindTexture(GL_TEXTURE_2D, 0);

	naiveTextures_[fileName] = texture;
	DEBUG_OUT("Finished loading " << fileName);
	return texture;
}

TextureLoader::~TextureLoader()
{
	for (auto it : naiveTextures_) {
		it.second->deleteTexture();
		SAFE_DELETE(it.second);
	}
	naiveTextures_.clear();
	for (auto it : nvddsImages_) {
		SAFE_DELETE(it.second);
	}
	nvddsImages_.clear();
}
