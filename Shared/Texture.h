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
			GLuint texId;
		};
	}
	namespace AZDO
	{
		/// AZDO uses sparse bindless texture arrays
		struct Texture {
			GLuint64 handle;
			GLuint64 page;
		};
	}
	namespace Vulk {
		struct Texture {
		};
	}
}
