#pragma once
#include "MemoryAllocation.h"

namespace QZL
{
	class DeviceMemory;

	template<typename DataType>
	class UniformBuffer {
	public:
		UniformBuffer(DeviceMemory& deviceMemory, MemoryAllocationPattern pattern, VkBufferUsageFlags flags, VkDeviceSize maxSize);
		~UniformBuffer();
		void uploadUniformRange(DataType* data, VkDeviceSize size, VkDeviceSize offset);

	private:
		MemoryAllocationDetails bufferDetails_;
		DeviceMemory& deviceMemory_;
	};

	template<typename DataType>
	inline UniformBuffer<DataType>::UniformBuffer(DeviceMemory& deviceMemory, MemoryAllocationPattern pattern, VkBufferUsageFlags flags, VkDeviceSize maxSize)
		: deviceMemory_(deviceMemory)
	{
		bufferDetails_ = deviceMemory.createBuffer(pattern, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | flags, maxSize);
		// TODO create staging buffer transfer alternative
		ENSURES(bufferDetails_.access == MemoryAccessType::kDirect || bufferDetails_.access == MemoryAccessType::kPersistant);
	}

	template<typename DataType>
	inline UniformBuffer<DataType>::~UniformBuffer()
	{
		deviceMemory_.deleteAllocation(bufferDetails_.id, bufferDetails_.buffer);
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
}
