#include "DeviceMemory.h"
#include "System.h"
#include "PhysicalDevice.h"
#include "LogicDevice.h"

using namespace QZL;

/*
typedef enum VkBufferUsageFlagBits {
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT = 0x00000001,
	VK_BUFFER_USAGE_TRANSFER_DST_BIT = 0x00000002,
	VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT = 0x00000004,
	VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00000008,
	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x00000010,
	VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00000020,
	VK_BUFFER_USAGE_INDEX_BUFFER_BIT = 0x00000040,
	VK_BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x00000080,
	VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT = 0x00000100,
	VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT = 0x00000800,
	VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT = 0x00001000,
	VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT = 0x00000200,
	VK_BUFFER_USAGE_RAY_TRACING_BIT_NV = 0x00000400,
	VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT = 0x00020000,
} VkBufferUsageFlagBits;
*/

DeviceMemory::DeviceMemory(const SystemDetails& sysDetails)
	: availableId_(1)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = sysDetails.physicalDevice->getPhysicalDevice();
	allocatorInfo.device = sysDetails.logicDevice->getLogicDevice();

	vmaCreateAllocator(&allocatorInfo, &allocator_);
}

DeviceMemory::~DeviceMemory()
{
	vmaDestroyAllocator(allocator_);
}

const MemoryAllocationDetails DeviceMemory::createBuffer(MemoryAllocationPattern pattern, VkBufferUsageFlags bufferUsage, VkDeviceSize size)
{
	MemoryAllocationDetails allocationDetails = {};
	allocationDetails.size = size;

	VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferCreateInfo.size = allocationDetails.size;
	bufferCreateInfo.usage = bufferUsage;

	VmaAllocationCreateInfo allocCreateInfo = makeVmaCreateInfo(pattern, allocationDetails.access);
	VmaAllocationInfo allocInfo;

	CHECK_VKRESULT(vmaCreateBuffer(allocator_, &bufferCreateInfo, &allocCreateInfo, &allocationDetails.buffer, &allocations_[availableId_++], &allocInfo));

	VkMemoryPropertyFlags memFlags;
	vmaGetMemoryTypeProperties(allocator_, allocInfo.memoryType, &memFlags);

	fixAccessType(allocationDetails.access, allocInfo, memFlags);

	return allocationDetails;
}

const MemoryAllocationDetails DeviceMemory::createImage(MemoryAllocationPattern pattern, VkImageCreateInfo imageCreateInfo, VkDeviceSize size)
{
	MemoryAllocationDetails allocationDetails = {};
	allocationDetails.size = size;

	VmaAllocationCreateInfo allocCreateInfo = makeVmaCreateInfo(pattern, allocationDetails.access);
	VmaAllocationInfo allocInfo;

	CHECK_VKRESULT(vmaCreateImage(allocator_, &imageCreateInfo, &allocCreateInfo, &allocationDetails.image, &allocations_[availableId_++], &allocInfo));

	VkMemoryPropertyFlags memFlags;
	vmaGetMemoryTypeProperties(allocator_, allocInfo.memoryType, &memFlags);

	fixAccessType(allocationDetails.access, allocInfo, memFlags);

	return allocationDetails;
}

void DeviceMemory::deleteAllocation(AllocationID id, VkBuffer buffer)
{
	vmaDestroyBuffer(allocator_, buffer, allocations_[id]);
	allocations_.erase(id);
}

void DeviceMemory::deleteAllocation(AllocationID id, VkImage image)
{
	vmaDestroyImage(allocator_, image, allocations_[id]);
	allocations_.erase(id);
}

void* DeviceMemory::mapMemory(const AllocationID& id)
{
	void* mappedData;
	CHECK_VKRESULT(vmaMapMemory(allocator_, allocations_[id], &mappedData));
	return mappedData;
}

void DeviceMemory::unmapMemory(const AllocationID & id)
{
	vmaUnmapMemory(allocator_, allocations_[id]);
}

void DeviceMemory::fixAccessType(MemoryAccessType& access, VmaAllocationInfo allocInfo, VkMemoryPropertyFlags memFlags)
{
	switch (access) {
	case MemoryAccessType::kPersistant:
		if (allocInfo.pUserData == nullptr)
			access = MemoryAccessType::kTransfer;
		break;
	case MemoryAccessType::kDirect:
		if ((memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
			access = MemoryAccessType::kTransfer;
		break;
	default:
		break;
	}
}

VmaAllocationCreateInfo DeviceMemory::makeVmaCreateInfo(MemoryAllocationPattern pattern, MemoryAccessType& access)
{
	VmaAllocationCreateInfo createInfo = {};
	switch (pattern) {
	case MemoryAllocationPattern::kRenderTarget:
		access = MemoryAccessType::kTransfer;
		createInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		break;
	case MemoryAllocationPattern::kStaticResource:
		access = MemoryAccessType::kTransfer;
		createInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		break;
	case MemoryAllocationPattern::kDynamicResource:
		access = access != MemoryAccessType::kPersistant ? MemoryAccessType::kDirect : access;
		createInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		break;
	case MemoryAllocationPattern::kReadback:
		access = access != MemoryAccessType::kPersistant ? MemoryAccessType::kDirect : access;
		createInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
		break;
	case MemoryAllocationPattern::kStaging:
		access = MemoryAccessType::kDirect;
		createInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		break;
	}

	switch (access) {
	case MemoryAccessType::kPersistant:
		createInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
		break;
	case MemoryAccessType::kDirect:
		createInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		break;
	}

	return createInfo;
}
