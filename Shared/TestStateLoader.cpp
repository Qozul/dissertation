/// Author: Ralph Ridley
/// Date: 16/1/19
#include "TestStateLoader.h"

using namespace QZL;
using namespace QZL::Shared;

void TestStateLoader::loadNaiveState()
{
	nbms_.push_back(meshLoader_->loadNaiveMesh("crate"));
	nbms_[0]->transform.setScale(0.02f);
}

void TestStateLoader::loadAZDOState()
{
}

void TestStateLoader::loadVulkanState()
{
}

QZL::Shared::TestStateLoader& QZL::Naive::testStateLoader()
{
	static QZL::Shared::TestStateLoader instance(QZL::Shared::TestStateLoader::TestState::kNaive);
	return instance;
}

QZL::Shared::TestStateLoader& QZL::AZDO::testStateLoader()
{
	static QZL::Shared::TestStateLoader instance(QZL::Shared::TestStateLoader::TestState::kAZDO);
	return instance;
}

QZL::Shared::TestStateLoader& QZL::Vulk::testStateLoader()
{
	static QZL::Shared::TestStateLoader instance(QZL::Shared::TestStateLoader::TestState::kVulkan);
	return instance;
}
