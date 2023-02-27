#pragma once

#include <string>
#include <vector>
#include "my_engine_device.hpp"
namespace lve {

	struct pipelineConfigInfo {

		pipelineConfigInfo() = default;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	};
	class LvePipeline {

	public:
		LvePipeline(LveDevice& Device,const std::string& vertFilePath,const std::string& fragFilePath,const pipelineConfigInfo& configInfo);
		~LvePipeline();

		LvePipeline(const LvePipeline&) = delete;
		LvePipeline& operator=(const LvePipeline&) = delete;

		static void defaultPipelineConfigInfo(pipelineConfigInfo& configInfo);
		void bind(VkCommandBuffer commandBuffer);

	private:
		static std::vector<char> readFile(const std::string& FilePath);

		void createGraphicsPipeline(const std::string& vertFilePath,const std::string& fragFilePath,const pipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		LveDevice& lveDevice; //Memory sensitive!!
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};

}