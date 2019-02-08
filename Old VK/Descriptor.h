#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	class Descriptor {
	public:
		Descriptor(VkDevice logicDevice, uint32_t count);
		~Descriptor();
		std::vector<VkDescriptorSet>& getSets();
		VkDescriptorSet* getSet(size_t index);
		VkDescriptorSetLayout getLayout();
		VkDescriptorPool getPool();
	private:
		void createSetLayout();
		void createPool();
		void createSets();

		VkDescriptorSetLayout layout_;
		VkDescriptorPool pool_;
		std::vector<VkDescriptorSet> sets_;
		uint32_t count_;

		VkDevice cLogicDevice_;
	};
}
