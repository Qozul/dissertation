#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace graphics
	{
		enum class ShaderType {
			kVertexShader = 0,
			kFragmentShader,
			kTesselationShader,
			kGeometryShader
		};
		class GraphicsPipeline {
		public:
			GraphicsPipeline(VkDevice logic_device, VkExtent2D swap_chain_extent, const VkFormat swap_chain_format, const std::string& vname, const std::string& fname);
			~GraphicsPipeline();
			VkPipelineLayout get_layout() noexcept;
			VkRenderPass get_render_pass() noexcept;
			VkPipeline get_pipeline() noexcept;
			static std::vector<char> read_spirv(const std::string& filename);
		private:
			void create_programmable_stages(VkDevice logic_device, const std::string& vname, const std::string& fname);
			void create_fixed_stages();
			VkShaderModule create_shader_module(VkDevice logic_device, const std::string& filename);

			VkPipelineLayout layout_;
			VkRenderPass render_pass_;
			VkPipeline pipeline_;
			VkDevice logic_device_;

			static const std::string kPath;
			static const std::string kExt;
		};
	}
}
