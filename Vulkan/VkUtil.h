#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
#define CHECK_VKRESULT(result) if (result != VK_SUCCESS) throw std::runtime_error("Check VK error code " + std::to_string(static_cast<int>(result)) \
+ " in file " + __FILE__ + " at line " + std::to_string(__LINE__));
#define NOTHROW_CHECK_VKRESULT(result) if (result != VK_SUCCESS) std::cout << "Check VK error code " << std::to_string(static_cast<int>(result)) \
	<< " in file" << __FILE__ << " at line " << __LINE__ << std::endl;

	/// Utility function for getting data from Vulkan where it needs to call a function to get
	/// a count of the data and then call it again to get the data.
	/// Can be called succinctly with the following basic line:
	/// auto x = Shared::Vulk::obtainVkData<R>(func, arg0, arg1, ..., argN);
	template<class R, typename FP, typename... Args>
	inline std::vector<R> obtainVkData(FP func_ptr, Args&&... args)
	{
		uint32_t count = 0;
		func_ptr(std::forward<Args>(args)..., &count, nullptr);
		DEBUG_IFERR(count == 0, ("Obtain vkdata found no data for type " + std::string(typeid(R).name())));
		std::vector<R> result(count);
		func_ptr(std::forward<Args>(args)..., &count, result.data());
		return result;
	}
}
