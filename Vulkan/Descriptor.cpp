#include "Descriptor.h"
#include "LogicDevice.h"

using namespace QZL;

Descriptor::Descriptor(const LogicDevice* logicDevice, const uint32_t maxSets)
	: logicDevice_(logicDevice)
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = maxSets;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = maxSets;

	CHECK_VKRESULT(vkCreateDescriptorPool(logicDevice_->getLogicDevice(), &poolInfo, nullptr, &pool_));
}

Descriptor::~Descriptor()
{
	vkDestroyDescriptorPool(logicDevice_->getLogicDevice(), pool_, nullptr);
	sets_.clear();
}

const VkDescriptorSet Descriptor::getSet(size_t idx)
{
	return sets_[idx];
}
