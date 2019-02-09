#pragma once
#include "VkUtil.h"

namespace QZL
{
	class Descriptor {
	public:
		Descriptor();
		~Descriptor();
	private:
		VkDescriptorSetLayout layout_;
		VkDescriptorPool pool_;
	};
}
