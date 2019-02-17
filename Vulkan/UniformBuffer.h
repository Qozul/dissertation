#pragma once
#include "LogicDevice.h"
#include "DeviceMemory.h"

namespace QZL
{
	class UniformBuffer {
	public:
		UniformBuffer(const LogicDevice* logicDevice, MemoryAllocationPattern pattern, uint32_t binding, VkBufferUsageFlags flags, VkDeviceSize maxSize, 
			VkShaderStageFlags stageFlags);
		~UniformBuffer();
		const VkDescriptorSetLayoutBinding& getBinding();
		template<typename DataType>
		void uploadUniformRange(DataType* data, VkDeviceSize size, VkDeviceSize offset);
		VkWriteDescriptorSet descriptorWrite(VkDescriptorSet set);
	private:
		MemoryAllocationDetails bufferDetails_;
		VkDeviceSize size_;
		uint32_t bindingIdx_;
		VkDescriptorSetLayoutBinding binding_;
		VkDescriptorBufferInfo bufferInfo_;
		const LogicDevice* logicDevice_;
	};

	template<typename DataType>
	inline void UniformBuffer::uploadUniformRange(DataType* data, VkDeviceSize size, VkDeviceSize offset)
	{
		auto deviceMemory = logicDevice_->getDeviceMemory();
		switch (bufferDetails_.access) {
		case MemoryAccessType::kPersistant:
			memcpy(bufferDetails_.mappedData, &data[offset], size);
			break;
		case MemoryAccessType::kDirect: {
			void* dataMap = deviceMemory->mapMemory(bufferDetails_.id);
			memcpy(dataMap, &data[offset], size);
			deviceMemory->unmapMemory(bufferDetails_.id);
			break;
		}
		case MemoryAccessType::kTransfer:
			// TODO
			ENSURES(false);
			break;
		}
	}
}
