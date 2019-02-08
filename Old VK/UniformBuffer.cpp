#include "UniformBuffer.h"

using namespace QZL;

UniformBuffer::UniformBuffer(VkPhysicalDevice physicalDevice, VkDevice logicDevice)
	: cLogicDevice_(logicDevice)
{
	size_ = sizeof(MVP);
	Shared::Vulk::createBuffer(physicalDevice, logicDevice, size_, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer_, memory_);
}

UniformBuffer::~UniformBuffer()
{
	if (buffer_ != VK_NULL_HANDLE) {
		vkDestroyBuffer(cLogicDevice_, buffer_, nullptr);
		vkFreeMemory(cLogicDevice_, memory_, nullptr);
	}
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
{
	buffer_ = other.buffer_;
	other.buffer_ = VK_NULL_HANDLE;
	memory_ = other.memory_;
	other.memory_ = VK_NULL_HANDLE;
	size_ = other.size_;
	other.size_ = 0;
	cLogicDevice_ = other.cLogicDevice_;
	other.cLogicDevice_ = VK_NULL_HANDLE;
}

void UniformBuffer::updateDescriptorSets(VkDescriptorSet set)
{
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = buffer_;
	bufferInfo.offset = 0;
	bufferInfo.range = size_;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = set;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(cLogicDevice_, 1, &descriptorWrite, 0, nullptr);
}

VkDeviceMemory UniformBuffer::getMemory()
{
	return memory_;
}
