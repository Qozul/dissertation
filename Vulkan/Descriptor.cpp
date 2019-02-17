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
	for (auto layout : layouts_)
		vkDestroyDescriptorSetLayout(logicDevice_->getLogicDevice(), layout, nullptr);
	vkDestroyDescriptorPool(logicDevice_->getLogicDevice(), pool_, nullptr);
	sets_.clear();
}

size_t Descriptor::createSets(const std::vector<VkDescriptorSetLayout>& layouts)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool_;
	allocInfo.descriptorSetCount = layouts.size();
	allocInfo.pSetLayouts = layouts.data();

	size_t firstIdx = sets_.size();
	sets_.resize(sets_.size() + layouts.size());
	CHECK_VKRESULT(vkAllocateDescriptorSets(logicDevice_->getLogicDevice(), &allocInfo, &sets_[firstIdx]));

	return firstIdx;
}

const VkDescriptorSet Descriptor::getSet(size_t idx)
{
	return sets_[idx];
}

VkDescriptorSetLayout Descriptor::makeLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();
	VkDescriptorSetLayout layout;
	CHECK_VKRESULT(vkCreateDescriptorSetLayout(logicDevice_->getLogicDevice(), &layoutInfo, nullptr, &layout));
	layouts_.push_back(layout);
	return layout;
}

void Descriptor::updateDescriptorSets(const std::vector<VkWriteDescriptorSet>& descriptorWrites)
{
	vkUpdateDescriptorSets(logicDevice_->getLogicDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}
