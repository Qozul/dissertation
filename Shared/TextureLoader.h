/// Author: Ralph Ridley
/// Date: 20/1/19
/// Purpose: To deal with all kinds of texture loading
#pragma once
#include "Utility.h"
#include "nv_dds.h"
#include "Texture.h"

namespace QZL
{
	namespace Shared
	{
		class TextureLoader {
		public:
			TextureLoader() = default;
			~TextureLoader();

			Naive::Texture* loadNaiveTexture(const std::string& fileName);
			//AZDO::Texture* loadAZDOTexture(const std::string& fileName);
			//Vulk::Texture* loadVulkanTexture(const std::string& fileName);
		private:
			std::map<std::string, nv_dds::CDDSImage*> nvddsImages_;
			std::map<std::string, Naive::Texture*> naiveTextures_;
			//std::vector<AZDO::Texture> azdoTextures_;
			//std::vector<Vulk::Texture> vulkTextures_;

			static const std::string kPath;
			static const std::string kExt;
		};
	}
}
