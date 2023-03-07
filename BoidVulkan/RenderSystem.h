#pragma once

#include "lve_pipeline.hpp"
#include "Lve_Window.hpp"
#include "my_engine_device.hpp"
#include "Renderer.h"
#include "Camera.h"
#include <memory>
#include<vector>
#include "GameObject.h"
#include"lve_frameInfo.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

namespace lve {



	class RenderSystem {
	public:

		RenderSystem(LveDevice &device,VkRenderPass renderPass);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;
		void renderGameObjects(frameInfo &frameInfo,std::vector<GameObject>& gameObjects);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		


		LveDevice &lveDevice;
		std::unique_ptr<LvePipeline> lvePipeline;

		VkPipelineLayout pipelineLayout;


	};
}
