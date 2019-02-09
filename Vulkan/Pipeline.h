#pragma once
#include "VkUtil.h"

namespace QZL
{
	class Pipeline {
	public:
		Pipeline();
		virtual ~Pipeline();
		virtual void submitCommands() = 0;
	private:
		VkPipeline pipeline_;
	};
}
