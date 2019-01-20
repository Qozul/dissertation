/// Author: Ralph Ridley
/// Date: 15/01/19
/// Purpose: Give all texture types in one place for easy comparison and testing
#pragma once
#include "Utility.h"

namespace QZL
{
	namespace Naive
	{
		struct Texture {
			// Must be a generated texture id
			GLuint id;
			void deleteTexture() {
				glDeleteTextures(1, &id);
			}
		};
	}
	namespace AZDO
	{
		/// AZDO uses sparse bindless texture arrays
		struct Texture {
			GLuint64 handle;
			GLuint64 page;
			void deleteTexture() {
			}
		};
	}
	namespace Vulk {
		struct Texture {
		};
	}
}
