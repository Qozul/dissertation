#include "TextureStore.h"

using namespace QZL;
using namespace QZL::AZDO;

TextureStore::TextureStore(const TexStoreInfo& info, GLint maxPages)
	: info_(info)
{
	GLint indexCount = 0;
	GLint xSize = 0;
	GLint ySize = 0;
	GLint zSize = 0;

	GLint bestIndex = -1;
	GLint bestXSize = 0;
	GLint bestYSize = 0;

	glGenTextures(1, &texId_);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texId_);

	// Sparse
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SPARSE_ARB, GL_TRUE);

	glGetInternalformativ(GL_TEXTURE_2D_ARRAY, info.format, GL_NUM_VIRTUAL_PAGE_SIZES_ARB, 1, &indexCount);

	ENSURES(indexCount > 0);

	for (GLint i = 0; i < indexCount; ++i) {
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, i);
		glGetInternalformativ(GL_TEXTURE_2D_ARRAY, info.format, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &xSize);
		glGetInternalformativ(GL_TEXTURE_2D_ARRAY, info.format, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &ySize);
		glGetInternalformativ(GL_TEXTURE_2D_ARRAY, info.format, GL_VIRTUAL_PAGE_SIZE_Z_ARB, 1, &zSize);

		// For our purposes, the "best" format is the one that winds up with Z=1 and the largest x and y sizes.
		if (zSize == 1) {
			if (xSize >= bestXSize && ySize >= bestYSize) {
				bestIndex = i;
				bestXSize = xSize;
				bestYSize = ySize;
			}
		}
	}
	// Setup what to do when the texture has to be scaled
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 8);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, bestIndex);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, info.levels, info.format, info.width, info.height, maxPages);

	// This would mean the implementation has no valid sizes for us, or that this format doesn't actually support sparse
	// texture allocation. Need to implement the fallback.
	ENSURES(bestIndex != -1);

	handle_ = glGetTextureHandleARB(texId_);
	ENSURES(handle_ != 0);

	glMakeTextureHandleResidentARB(handle_);
	
	for (GLsizei i = 0; i < maxPages; ++i) {
		freePages_.push(i);
	}
}

TextureStore::~TextureStore()
{
	// Uncommit all the memory
	for (GLsizei page : pages_) {
		commit(page, false);
	}
	glMakeTextureHandleNonResidentARB(handle_);
	glDeleteTextures(1, &texId_);
}

bool TextureStore::hasSpace()
{
	return freePages_.size() > 0;
}

void TextureStore::commit(GLsizei page, bool commit)
{
	Shared::checkGLError();
	GLsizei mipLevelWidth = info_.width;
	GLsizei mipLevelHeight = info_.height;
	// For each mipmap level commit or uncommit the texture in the layer
	glBindTexture(GL_TEXTURE_2D_ARRAY, texId_);
	for (GLsizei mipLevel = 0; mipLevel < info_.levels; ++mipLevel) {
		glTexPageCommitmentARB(GL_TEXTURE_2D_ARRAY, mipLevel, 0, 0, page, mipLevelWidth, mipLevelHeight, 1, commit);
		mipLevelWidth = std::max(mipLevelWidth / 2, 1);
		mipLevelHeight = std::max(mipLevelHeight / 2, 1);
	}
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TextureStore::free(GLsizei page)
{
	pages_.erase(std::remove(pages_.begin(), pages_.end(), page), pages_.end());
	freePages_.push(page);
}

GLsizei TextureStore::alloc()
{
	GLsizei page = -1;
	if (freePages_.size() > 0) {
		// Use the first free page
		page = freePages_.front();
		freePages_.pop();
	}
	pages_.push_back(page);
	return page;
}

void TextureStore::addSubImage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, 
	GLsizei depth, GLenum format, GLsizei image_size, const void* data)
{
	ENSURES(data != nullptr);
	if (level >= info_.levels)
		return;

	glCompressedTextureSubImage3D(texId_, level, xoffset, yoffset, zoffset, width, height, depth, format, image_size, data);
	Shared::checkGLError();
}

TexStoreInfo QZL::AZDO::makeTexStoreInfo(GLsizei levels, GLenum format, GLsizei width, GLsizei height)
{
	TexStoreInfo info = {};
	info.levels = levels;
	info.format = format;
	info.width = width;
	info.height = height;
	return info;
}
