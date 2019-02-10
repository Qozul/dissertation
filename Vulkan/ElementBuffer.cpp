#include "ElementBuffer.h"
#include "DeviceMemory.h"

using namespace QZL;

ElementBuffer::ElementBuffer(DeviceMemory& deviceMemory)
	: deviceMemory_(deviceMemory), isCommitted_(false)
{
}

ElementBuffer::~ElementBuffer()
{
	if (isCommitted_)
		deviceMemory_.deleteAllocation(vertexBufferDetails_.id, vertexBufferDetails_.buffer);
		deviceMemory_.deleteAllocation(indexBufferDetails_.id, indexBufferDetails_.buffer);
}

void ElementBuffer::commit()
{
	if (isCommitted_)
		return;

	size_t size = indices_.size() * sizeof(uint16_t);
	size_t size2 = vertices_.size() * sizeof(Shared::Vertex);
	size_t largestSize = size > size2 ? size : size2;
	MemoryAllocationDetails stagingBuffer = deviceMemory_.createBuffer(MemoryAllocationPattern::kStaging, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, largestSize);

	void* data = deviceMemory_.mapMemory(stagingBuffer.id);
	memcpy(data, vertices_.data(), size2);
	deviceMemory_.unmapMemory(stagingBuffer.id);

	vertexBufferDetails_ = deviceMemory_.createBuffer(MemoryAllocationPattern::kStaticResource, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size2);

	// TODO invoke transfer

	void* data = deviceMemory_.mapMemory(stagingBuffer.id);
	memcpy(data, indices_.data(), size);
	deviceMemory_.unmapMemory(stagingBuffer.id);

	indexBufferDetails_ = deviceMemory_.createBuffer(MemoryAllocationPattern::kStaticResource, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, size);

	// TODO invoke transfer

	deviceMemory_.deleteAllocation(stagingBuffer.id, stagingBuffer.buffer);

	isCommitted_ = true;
}

size_t ElementBuffer::addVertices(Shared::Vertex* data, const size_t size)
{
	ENSURES(!isCommitted_);
	const size_t prevSize = vertices_.size();
	vertices_.resize(prevSize + size);
	std::copy_n(data, size, vertices_.begin() + prevSize);
	return prevSize;
}

size_t ElementBuffer::addIndices(uint16_t* data, const size_t size)
{
	ENSURES(!isCommitted_);
	const size_t prevSize = indices_.size();
	indices_.resize(prevSize + size);
	std::copy_n(data, size, indices_.begin() + prevSize);
	return prevSize;
}

VkBuffer ElementBuffer::getVertexBuffer()
{
	if (isCommitted_)
		return vertexBufferDetails_.buffer;
	else
		return VK_NULL_HANDLE;
}

VkBuffer ElementBuffer::getIndexBuffer()
{
	if (isCommitted_)
		return indexBufferDetails_.buffer;
	else
		return VK_NULL_HANDLE;
}
