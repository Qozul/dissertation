#pragma once
#include "../Shared/Utility.h"

namespace nv_dds
{
	class CDDSImage;
}
namespace QZL
{
	namespace AZDO
	{
		class TextureStore;

		struct TextureData
		{
			GLuint64 handle;
			GLfloat page;
		};

		/// AZDO uses sparse bindless texture arrays
		class Texture {
		public:
			Texture(TextureStore* store, nv_dds::CDDSImage& image);
			~Texture();
			Texture(const Texture& other) = delete;
			Texture(Texture&& other) = delete;
			// Commits the texture to physical memory
			void commit(bool commit);

			GLuint64 getHandle() {
				return data_.handle;
			}
			GLfloat getPage() {
				return data_.page;
			}

			TextureData data() {
				return data_;
			}

		private:
			void addToStorage(nv_dds::CDDSImage& image);

			TextureStore* store_;
			TextureData data_;

			static const std::string kPath;
			static const std::string kExt;
		};
	}
}
