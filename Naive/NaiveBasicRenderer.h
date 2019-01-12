/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once
#include "NaiveAbstractRenderer.h"

namespace QZL
{
	namespace Naive
	{
		class BasicRenderer : public AbstractRenderer {
		public:
			BasicRenderer(ShaderPipeline* pipeline);
			void doFrame();
		};
	}
}
