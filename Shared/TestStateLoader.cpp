/// Author: Ralph Ridley
/// Date: 16/1/19
#include "TestStateLoader.h"

using namespace QZL;
using namespace QZL::Shared;

NaiveStateLoader& QZL::Naive::testStateLoader()
{
	static QZL::Shared::TestStateLoader<Naive::BasicMesh, Naive::TexturedBasicMesh> instance;
	if (!instance.naiveLoaded_)
		instance.loadNaiveState();
	return instance;
}
