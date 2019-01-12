/// Author: Ralph Ridley
/// Date: 31/12/18
#pragma once

#include "ShaderPipeline.h"
#include "Mesh.h"
#include "Texture.h"

namespace QZL
{
	class RendererBase {
	public:
		// pipeline must by dynamically allocated
		RendererBase(ShaderPipeline* pipeline) : pipeline_(pipeline) {};
		virtual ~RendererBase() {
			delete pipeline_;
		}
		virtual void doFrame() = 0;
	protected:
		ShaderPipeline* pipeline_;
	};
}
