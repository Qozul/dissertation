/// Author: Ralph Ridley
/// Date: 10/1/19
#pragma once
#include "NaiveAbstractRenderer.h"

namespace QZL
{
	namespace Naive
	{
		class TexturedRenderer : public AbstractRenderer {
		public:
			TexturedRenderer(ShaderPipeline* pipeline);
			void doFrame();
			void addTexturedMesh(Texture* texture, Mesh* mesh) {
				texturedMeshes_[texture].push_back(mesh);
			};
		private:
			std::map<Texture*, std::vector<Mesh*>> texturedMeshes_;
		};
	}
}