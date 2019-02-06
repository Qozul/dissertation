#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace AZDO
	{
		// Textures valid in a store can be determined by the number of mip levels, format, width, and height.
		struct TexStoreInfo {
			GLsizei levels;
			GLenum format;
			GLsizei width;
			GLsizei height;
			friend bool operator==(const TexStoreInfo& info0, const TexStoreInfo& info1) {
				return info0.levels == info1.levels && info0.format == info1.format &&
					info0.width == info1.width && info0.height == info1.height;
			}
		};
		TexStoreInfo makeTexStoreInfo(GLsizei levels, GLenum format, GLsizei width, GLsizei height);

		class TextureStore {
			friend class Texture;
			friend class TextureLoader;
		public:
			~TextureStore();
		private:
			TextureStore(const TexStoreInfo& info, GLint maxPages);

			bool hasSpace();
			void commit(GLsizei page, bool commit);
			void free(GLsizei page);
			GLsizei alloc();
			void addSubImage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
				GLsizei height, GLsizei depth, GLenum format, GLsizei image_size, const void* data);

			static GLuint64 obtainTexStore(const TexStoreInfo info);

			GLuint64 handle_;
			GLuint texId_;
			const TexStoreInfo info_;
			std::queue<GLsizei> freePages_;
			std::vector<GLsizei> pages_;
		};
	}
}