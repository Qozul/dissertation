#include "TextureLoader.h"
#include "Texture.h"
#include "TextureStore.h"
#include "../Shared/nv_dds.h"

using namespace QZL;
using namespace AZDO;

const std::string TextureLoader::kPath = "../Assets/Textures/";
const std::string TextureLoader::kExt = ".dds";

TextureLoader::TextureLoader()
{
	glGetIntegerv(GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS_ARB, &maxPages_);
}

TextureLoader::~TextureLoader()
{
	for (auto& store : stores_) {
		SAFE_DELETE(store);
	}
}

Texture* TextureLoader::loadTexture(const std::string& fileName)
{
	DEBUG_OUT("Loading sparse bindless texture " << fileName);

	nv_dds::CDDSImage image;
	image.load(kPath + fileName + kExt);
	// If this is not +1 then everything dies
	auto info = makeTexStoreInfo(image.get_num_mipmaps()+1, image.get_format(), image.get_width(), image.get_height());
	auto store = obtainTexStore(info);

	return new Texture(store, image);
}

TextureStore* TextureLoader::obtainTexStore(const TexStoreInfo& info)
{
	// If a suitable store exists then use it
	for (auto& store : stores_) {
		if (store->info_ == info && store->hasSpace()) {
			return store;
		}
	}
	// No suitable store found so create one
	TextureStore* store = new TextureStore(info, maxPages_);
	stores_.push_back(store);
	return store;
}
