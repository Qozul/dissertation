/// Author: Ralph Ridley
/// Date: 29/10/18
/// Purpose: Defininitions for VertexUtility members
#include "Vertex.h"

using namespace QZL;

VkVertexInputBindingDescription VertexUtility::fillBindDesc(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
	VkVertexInputBindingDescription bindingDesc = {};
	bindingDesc.binding = binding;
	bindingDesc.stride = stride;
	bindingDesc.inputRate = inputRate;
	return bindingDesc;
}

void VertexUtility::fillAttribDescs(VkVertexInputAttributeDescription& attribDesc, uint32_t location, uint32_t binding, VkFormat format, uint32_t offset)
{
	attribDesc.location = location;
	attribDesc.binding = binding;
	attribDesc.format = format;
	attribDesc.offset = offset;
}
