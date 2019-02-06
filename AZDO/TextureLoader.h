#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace AZDO
	{
		class TextureStore;
		class Texture;
		class TexStoreInfo;

		class TextureLoader {
		public:
			TextureLoader();
			// Undefined behaviour for stores to be deleted without deleting the textures first.
			// Stores are deleted here, therefore do not delete the loader without first cleaning any textures
			// created with it.
			~TextureLoader();
			Texture* loadTexture(const std::string& fileName);
		private:
			TextureStore* obtainTexStore(const TexStoreInfo& info);

			std::vector<TextureStore*> stores_;

			GLint maxPages_;

			static const std::string kPath;
			static const std::string kExt;
		};
	}
}
