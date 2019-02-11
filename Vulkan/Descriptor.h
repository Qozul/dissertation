#pragma once
#include "VkUtil.h"

namespace QZL
{
	class LogicDevice;

	class Descriptor {
	public:
		Descriptor(const LogicDevice* logicDevice, const uint32_t maxSets);
		~Descriptor();
		const VkDescriptorSet getSet(size_t idx);
	private:
		VkDescriptorPool pool_;
		std::vector<VkDescriptorSet> sets_;
		const LogicDevice* logicDevice_;
	};
}
