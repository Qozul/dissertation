#include "..\AZDO\Texture.h"
#include "Texture.h"
#include "../Shared/nv_dds.h"

using namespace QZL;
using namespace QZL::Naive;

const std::string Texture::kPath = "../Assets/Textures/";
const std::string Texture::kExt = ".dds";

Texture::Texture(const std::string& fileName)
{
	DEBUG_OUT("Loading texture " << fileName);

	glGenTextures(1, &id);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id);

	nv_dds::CDDSImage image;
	image.load(kPath + fileName + kExt);
	image.upload_texture2D();

	glBindTexture(GL_TEXTURE_2D, 0);

	DEBUG_OUT("Finished loading " << fileName);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}
