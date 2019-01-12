#include "Descriptor.h"

using namespace QZL;

Descriptor::Descriptor(VkDevice logicDevice, uint32_t count)
	: cLogicDevice_(logicDevice), count_(count)
{
	createSetLayout();
	createPool();
	createSets();
}

Descriptor::~Descriptor()
{
	vkDestroyDescriptorPool(cLogicDevice_, pool_, nullptr);
	vkDestroyDescriptorSetLayout(cLogicDevice_, layout_, nullptr);
}

std::vector<VkDescriptorSet>& QZL::Descriptor::getSets()
{
	return sets_;
}

VkDescriptorSet* Descriptor::getSet(size_t index)
{
	return &sets_.at(index);
}

VkDescriptorSetLayout Descriptor::getLayout()
{
	return layout_;
}

VkDescriptorPool Descriptor::getPool()
{
	return pool_;
}

void Descriptor::createSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	CHECK_VKRESULT(vkCreateDescriptorSetLayout(cLogicDevice_, &layoutInfo, nullptr, &layout_));
}

void Descriptor::createPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = count_;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = count_;

	CHECK_VKRESULT(vkCreateDescriptorPool(cLogicDevice_, &poolInfo, nullptr, &pool_));
}

void Descriptor::createSets() {
	std::vector<VkDescriptorSetLayout> layouts(count_, layout_);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool_;
	allocInfo.descriptorSetCount = count_;
	allocInfo.pSetLayouts = layouts.data();

	sets_.resize(count_);
	CHECK_VKRESULT(vkAllocateDescriptorSets(cLogicDevice_, &allocInfo, sets_.data()));
}
