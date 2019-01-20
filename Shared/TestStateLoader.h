/// Author: Ralph Ridley
/// Date: 16/1/19
/// Purpose: To centralise and provide consistency for tests between the different renderers
#pragma once
#include "MeshLoader.h"
#include "TextureLoader.h"
#include "Texture.h"

namespace QZL
{
	namespace Shared
	{
		template<typename BMesh, typename TMesh>
		class TestStateLoader;
	}
	using NaiveStateLoader = QZL::Shared::TestStateLoader<Naive::BasicMesh, Naive::TexturedBasicMesh>;
	//using AZDOStateLoader = QZL::Shared::TestStateLoader<AZDO::BasicMesh, AZDO::TexturedBasicMesh>;
	//using VulkStateLoader = QZL::Shared::TestStateLoader<Vulk::BasicMesh, Vulk::TexturedBasicMesh>;
	namespace Naive
	{
		NaiveStateLoader& testStateLoader();
	}
	namespace AZDO
	{
		//QZL::Shared::TestStateLoader& testStateLoader();
	}

	namespace Vulk
	{
		//QZL::Shared::TestStateLoader& testStateLoader();
	}
	namespace Shared
	{
		template<typename BMesh, typename TMesh>
		class TestStateLoader {
			friend NaiveStateLoader& QZL::Naive::testStateLoader();
			//friend QZL::Shared::TestStateLoader& QZL::AZDO::testStateLoader();
			//friend QZL::Shared::TestStateLoader& QZL::Vulk::testStateLoader();
		public:
			std::vector<BMesh*> getBasicMeshes() {
				return basicMeshes_;
			}
			std::map<GLuint, std::vector<TMesh*>> getTexturedMeshes() {
				return texturedMeshes_;
			}
		private:
			TestStateLoader()
				: meshLoader_(new MeshLoader()), textureLoader_(new TextureLoader()), naiveLoaded_(false)
			{
			}
			~TestStateLoader() {
				for (auto mesh : basicMeshes_)
					SAFE_DELETE(mesh);
				for (auto it : texturedMeshes_)
					for (auto mesh : it.second)
						SAFE_DELETE(mesh);

				SAFE_DELETE(meshLoader_);
				SAFE_DELETE(textureLoader_);
			}
			void loadNaiveState() {
				basicMeshes_.push_back(meshLoader_->loadNaiveMesh("crate"));
				basicMeshes_[0]->transform.setScale(0.02f);
				Naive::TexturedBasicMesh* textured = basicToTextured(*basicMeshes_[0]);
				textured->texture = textureLoader_->loadNaiveTexture("Mandelbrot");
				texturedMeshes_[textured->texture->id].push_back(textured);
				naiveLoaded_ = true;
			}
			void loadAZDOState();
			void loadVulkanState();

			bool naiveLoaded_;

			MeshLoader* meshLoader_;
			TextureLoader* textureLoader_;

			std::vector<BMesh*> basicMeshes_;
			// Map allows grouping textures together if wanted
			std::map<GLuint, std::vector<TMesh*>> texturedMeshes_;
		};
	}
}
