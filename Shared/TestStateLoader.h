/// Author: Ralph Ridley
/// Date: 16/1/19
/// Purpose: To centralise and provide consistency for tests between the different renderers
#pragma once
#include "MeshLoader.h"

namespace QZL
{
	namespace Shared
	{
		class TestStateLoader;
	}
	namespace Naive
	{
		QZL::Shared::TestStateLoader& testStateLoader();
	}
	namespace AZDO
	{
		QZL::Shared::TestStateLoader& testStateLoader();
	}

	namespace Vulk
	{
		QZL::Shared::TestStateLoader& testStateLoader();
	}
	namespace Shared
	{
		class TestStateLoader {
			friend QZL::Shared::TestStateLoader& QZL::Naive::testStateLoader();
			friend QZL::Shared::TestStateLoader& QZL::AZDO::testStateLoader();
			friend QZL::Shared::TestStateLoader& QZL::Vulk::testStateLoader();
		public:
			enum class TestState {
				kNaive, kAZDO, kVulkan
			};
			std::vector<Naive::BasicMesh*> getNaiveBasicMeshes() {
				return nbms_;
			}
		private:
			TestStateLoader(TestState state)
				: meshLoader_(new MeshLoader())
			{
				if (state == TestState::kNaive)
					loadNaiveState();
				else if (state == TestState::kAZDO)
					loadAZDOState();
				else
					loadVulkanState();
			}
			~TestStateLoader() {
				for (auto& mesh : nbms_)
					SAFE_DELETE(mesh);

				SAFE_DELETE(meshLoader_);
			}
			void loadNaiveState();
			void loadAZDOState();
			void loadVulkanState();

			MeshLoader* meshLoader_;

			std::vector<Naive::BasicMesh*> nbms_;
		};
	}
}
