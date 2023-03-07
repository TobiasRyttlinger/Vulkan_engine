#include "RenderSystem.h"

#include <stdexcept>
#include "glm/gtc/constants.hpp"
#include <array>
#include <stdexcept>
namespace lve {

	struct pushConstants {
		glm::mat4 transform{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};



	RenderSystem::RenderSystem(LveDevice& device, VkRenderPass renderPass) : lveDevice{ device } {
		createPipelineLayout();
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}


	void RenderSystem::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(pushConstants);

		VkPipelineLayoutCreateInfo pipeLineLayoutInfo{};
		pipeLineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeLineLayoutInfo.setLayoutCount = 0;
		pipeLineLayoutInfo.pSetLayouts = nullptr;
		pipeLineLayoutInfo.pushConstantRangeCount = 1;
		pipeLineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(lveDevice.device(), &pipeLineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}

	}


	void RenderSystem::createPipeline(VkRenderPass renderPass) {

		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
		pipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			"shaders/simpleShader.vert.spv",
			"shaders/simpleShader.frag.spv",
			pipelineConfig
			);
	}



	void RenderSystem::renderGameObjects(frameInfo &frameInfo,std::vector<GameObject>& gameObjects) {
		lvePipeline->bind(frameInfo.commandBuffer);

		auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();
		for (auto& obj : gameObjects) {

			pushConstants push{};
			auto modelMatrix = obj.transform.mat4();

			push.transform = projectionView * modelMatrix;
			push.normalMatrix = obj.transform.normalMatrix();
			vkCmdPushConstants(frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(pushConstants),
				&push);
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
	
		}
	}

}
