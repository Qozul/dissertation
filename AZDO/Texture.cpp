#include "Texture.h"
#include "../Shared/nv_dds.h"
#include "TextureStore.h"

using namespace QZL;
using namespace QZL::AZDO;

Texture::Texture(TextureStore* store, nv_dds::CDDSImage& image)
	: store_(store)
{
	data_.handle = store->handle_;
	data_.page = store_->alloc();
	addToStorage(image);
}

Texture::~Texture()
{
	commit(false);
	store_->free(data_.page);
	data_.page = -1;
}

void Texture::commit(bool commit)
{
	store_->commit(data_.page, commit);
}

void Texture::addToStorage(nv_dds::CDDSImage& image)
{
	// Only a physical texture can be written to
	commit(true);
	// Add the main image to storage
	Shared::checkGLError();
	ENSURES(true);
	store_->addSubImage(0, 0, 0, data_.page, image.get_width(), image.get_height(), 1, image.get_format(), image.get_size(), image);
	// Now add the mipmap levels
	for (unsigned int i = 0; i < image.get_num_mipmaps(); i++) {
		nv_dds::CSurface mipmap = image.get_mipmap(i);
		store_->addSubImage(static_cast<GLint>(i + 1), 0, 0, data_.page, mipmap.get_width(), mipmap.get_height(), 1,
			image.get_format(), mipmap.get_size(), mipmap);
	}
	commit(false);
}