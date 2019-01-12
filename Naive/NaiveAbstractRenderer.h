/// Author: Ralph Ridley
/// Date: 10/1/19
#pragma once

#include "RendererBase.h"

namespace QZL
{
	namespace Naive
	{
		class AbstractRenderer : public RendererBase {
		public:
			AbstractRenderer(ShaderPipeline* pipeline) : RendererBase(pipeline) {};
			virtual ~AbstractRenderer() {};
			// Mesh is not owned by this class and must be allocated
			void addMesh(Mesh* mesh) {
				meshes_.push_back(mesh);
			}
		protected:
			std::vector<Mesh*> meshes_;
		};
	}
}
