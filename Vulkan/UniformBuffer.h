#pragma once
#include "LogicDevice.h"
#include "DeviceMemory.h"

namespace QZL
{
	template<typename DataType>
	class UniformBuffer {
	public:
		UniformBuffer(const LogicDevice* logicDevice, MemoryAllocationPattern pattern, VkBufferUsageFlags flags, VkDeviceSize maxSize, 
			VkShaderStageFlags stageFlags);
		~UniformBuffer();
		void setDescriptorSet(VkDescriptorSet set);
		void uploadUniformRange(DataType* data, VkDeviceSize size, VkDeviceSize offset);
		VkDescriptorSetLayout getLayout();
	private:
		MemoryAllocationDetails bufferDetails_;
		VkDescriptorSetLayout layout_;
		VkDescriptorSet set_;
		const LogicDevice* logicDevice_;
	};

	template<typename DataType>
	inline UniformBuffer<DataType>::UniformBuffer(const LogicDevice* logicDevice, MemoryAllocationPattern pattern, 
		VkBufferUsageFlags flags, VkDeviceSize maxSize, VkShaderStageFlags stageFlags)
		: logicDevice_(logicDevice), set_(nullptr)
	{
		bufferDetails_ = logicDevice_->getDeviceMemory()->createBuffer(pattern, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | flags, maxSize);
		// TODO create staging buffer transfer alternative
		ENSURES(bufferDetails_.access == MemoryAccessType::kDirect || bufferDetails_.access == MemoryAccessType::kPersistant);

		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = 0;
		layoutBinding.descriptorCount = 1;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		layoutBinding.pImmutableSamplers = nullptr;
		layoutBinding.stageFlags = stageFlags;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &layoutBinding;

		CHECK_VKRESULT(vkCreateDescriptorSetLayout(logicDevice_, &layoutInfo, nullptr, &layout_));
	}

	template<typename DataType>
	inline UniformBuffer<DataType>::~UniformBuffer()
	{
		deviceMemory_.deleteAllocation(bufferDetails_.id, bufferDetails_.buffer);
		vkDestroyDescriptorSetLayout(logicDevice_, layout_, nullptr);
	}

	template<typename DataType>
	inline void UniformBuffer<DataType>::setDescriptorSet(VkDescriptorSet set)
	{
		set_ = set;
	}

	template<typename DataType>
	inline void UniformBuffer<DataType>::uploadUniformRange(DataType* data, VkDeviceSize size, VkDeviceSize offset)
	{
		switch (bufferDetails_.access) {
		case MemoryAccessType::kPersistant:
			memcpy(bufferDetails_.mappedData, data[offset], sizeof(DataType) * size);
			break;
		case MemoryAccessType::kDirect:
			void* dataMap = deviceMemory_.mapMemory(bufferDetails_.id);
			memcpy(dataMap, data[offset], sizeof(DataType) * size);
			deviceMemory_.unmapMemory(bufferDetails_.id);
			break;
		case MemoryAccessType::kTransfer:
			// TODO
			break;
		}
	}
	template<typename DataType>
	inline VkDescriptorSetLayout UniformBuffer<DataType>::getLayout()
	{
		return layout_;
	}
}
