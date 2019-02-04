#include "Texture.h"
#include "../Shared/nv_dds.h"
#include "TextureStore.h"

#define STORE TextureStore::sExistingStores[data_.handle]

using namespace QZL;
using namespace QZL::AZDO;

const std::string Texture::kPath = "../Assets/Textures/";
const std::string Texture::kExt = ".dds";

Texture::Texture(const std::string& fileName)
{
	DEBUG_OUT("Loading sparse bindless texture " << fileName);

	nv_dds::CDDSImage image;
	image.load(kPath + fileName + kExt);

	data_.handle = TextureStore::obtainTexStore(makeTexStoreInfo(image.get_num_mipmaps(), image.get_format(), image.get_width(), image.get_height()));
	data_.page = STORE->alloc();
	addToStorage(image);

	DEBUG_OUT("Finished loading " << fileName);
}

Texture::~Texture()
{
	commit(false);
	STORE->free(data_.page);
	data_.page = -1;
}

void Texture::commit(bool commit)
{
	STORE->commit(data_.page, commit);
}

void Texture::addToStorage(nv_dds::CDDSImage& image)
{
	// Only a committed texture can be written to
	commit(true);
	// Add the main image to storage
	STORE->addSubImage(0, 0, 0, data_.page, image.get_width(), image.get_height(), 1, image.get_format(), image.get_size(), image);
	// Now add the mipmap levels
	for (unsigned int i = 0; i < image.get_num_mipmaps(); i++) {
		nv_dds::CSurface mipmap = image.get_mipmap(i);
		STORE->addSubImage(static_cast<GLint>(i + 1), 0, 0, data_.page, mipmap.get_width(), mipmap.get_height(), 1,
			image.get_format(), mipmap.get_size(), mipmap);
	}
	// Don't want to leave it on physical memory
	commit(false);
}