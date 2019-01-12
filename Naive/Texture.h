/// Author: Ralph Ridley
/// Date: 10/1/19

#pragma once
//#include "Utility.h"
#include "../Shared/Utility.h"

namespace QZL
{
	namespace Naive
	{
		struct Texture {
			GLuint id;
		};
	}
	namespace AZDO
	{
		struct Texture {
			GLuint64 handle;
		};
	}
}
