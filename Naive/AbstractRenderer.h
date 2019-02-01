/// Author: Ralph Ridley
/// Date: 01/02/19
#pragma once
#include "../Shared/RendererBase.h"

namespace QZL
{
	namespace Naive
	{
		// Intermediate renderer for shared behaviour of non-textured renderers.
		class AbstractRenderer : public Shared::RendererBase {
		public:
			AbstractRenderer(ShaderPipeline* pipeline)
				: Shared::RendererBase(pipeline) {}
			~AbstractRenderer() {
				for (auto& mesh : meshes_) {
					SAFE_DELETE(mesh);
				}
			}
			void addMesh(BasicMesh* mesh) {
				meshes_.push_back(mesh);
			}
			void addMesh(std::vector<BasicMesh*> meshes) {
				std::copy(std::begin(meshes), std::end(meshes), std::back_inserter(meshes_));
			}
		protected:
			std::vector<BasicMesh*> meshes_;
		};
	}
}
