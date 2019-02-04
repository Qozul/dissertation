#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace Naive
	{
		class Texture {
		public:
			Texture(const std::string& fileName);
			~Texture();
			// Must be a generated texture id
			GLuint id;
		private:
			static const std::string kPath;
			static const std::string kExt;
		};
	}
}
