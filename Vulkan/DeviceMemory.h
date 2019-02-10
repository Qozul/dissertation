#pragma once
#include "vk_mem_alloc.h"
#include "MemoryAllocation.h"

namespace QZL
{
	struct SystemDetails;

	/// Interface for https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/index.html
	class DeviceMemory {
		friend class LogicDevice;
	public:
		const MemoryAllocationDetails createBuffer(MemoryAllocationPattern pattern, VkBufferUsageFlags bufferUsage, VkDeviceSize size);
		const MemoryAllocationDetails createImage(MemoryAllocationPattern pattern, VkImageCreateInfo imageCreateInfo, VkDeviceSize size);
		void deleteAllocation(AllocationID id, VkBuffer buffer);
		void deleteAllocation(AllocationID id, VkImage image);
		void* mapMemory(const AllocationID& id);
		void unmapMemory(const AllocationID& id);
		void transferMemory(const AllocationID& srcId, const AllocationID& dstId);
	private:
		DeviceMemory(const SystemDetails& sysDetails);
		~DeviceMemory();

		// Ensure mapped access is possible if requested
		void fixAccessType(MemoryAccessType& access, VmaAllocationInfo allocInfo, VkMemoryPropertyFlags memFlags);
		// Choose VmaCreateInfo based on the selected pattern
		VmaAllocationCreateInfo makeVmaCreateInfo(MemoryAllocationPattern pattern, MemoryAccessType& access);

		VmaAllocator allocator_;
		AllocationID availableId_; // 0 reserved for invalid id
		std::map<AllocationID, VmaAllocation> allocations_;
	};
}
