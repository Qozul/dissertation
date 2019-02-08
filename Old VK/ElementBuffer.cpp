#include "ElementBuffer.h"

using namespace QZL;

VertexBuffer::VertexBuffer(VkPhysicalDevice physicalDevice, VkDevice logicDevice, VkCommandPool commandPool, VkQueue queue)
	: cLogicDevice_(logicDevice)
{

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	Shared::Vulk::createBuffer(physicalDevice, cLogicDevice_, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(cLogicDevice_, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(cLogicDevice_, stagingBufferMemory);

	Shared::Vulk::createBuffer(physicalDevice, cLogicDevice_, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_, memory_);

	Shared::Vulk::copyBuffer(cLogicDevice_, commandPool, queue, stagingBuffer, buffer_, bufferSize);

	vkDestroyBuffer(cLogicDevice_, stagingBuffer, nullptr);
	vkFreeMemory(cLogicDevice_, stagingBufferMemory, nullptr);
}

VertexBuffer::~VertexBuffer()
{
	vkDestroyBuffer(cLogicDevice_, buffer_, nullptr);
	vkFreeMemory(cLogicDevice_, memory_, nullptr);
}

VkBuffer VertexBuffer::get()
{
	return buffer_;
}

IndexBuffer::IndexBuffer(VkPhysicalDevice physicalDevice, VkDevice logicDevice, VkCommandPool commandPool, VkQueue queue)
	: cLogicDevice_(logicDevice)
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	Shared::Vulk::createBuffer(physicalDevice, cLogicDevice_, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(cLogicDevice_, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(cLogicDevice_, stagingBufferMemory);

	Shared::Vulk::createBuffer(physicalDevice, cLogicDevice_, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_, memory_);

	Shared::Vulk::copyBuffer(cLogicDevice_, commandPool, queue, stagingBuffer, buffer_, bufferSize);

	vkDestroyBuffer(cLogicDevice_, stagingBuffer, nullptr);
	vkFreeMemory(cLogicDevice_, stagingBufferMemory, nullptr);
}

IndexBuffer::~IndexBuffer()
{
	vkDestroyBuffer(cLogicDevice_, buffer_, nullptr);
	vkFreeMemory(cLogicDevice_, memory_, nullptr);
}

VkBuffer IndexBuffer::get()
{
	return buffer_;
}
