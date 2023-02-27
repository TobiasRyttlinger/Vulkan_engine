#pragma once

#include "Lve_Window.hpp"
#include "my_engine_device.hpp"
#include "lve_swap_chain.hpp"

#include <memory>
#include<vector>
#include<cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

namespace lve {

	class Renderer {
	public:

		Renderer(LveWindow& lveWindow, LveDevice& lveDevice);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const {
			return lveSwapcChain->getRenderPass();
		}
		float getAspectRatio() const { return lveSwapcChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get commandBuffer if frame is not being progressed!");
			return commandBuffer[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frameIndex if frame is not being progressed!");
			return currentFrameIndex;
		}
		VkCommandBuffer beginFrame();
		void EndFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	private:

		void createCommandBuffer();

		void recreateSwapChain();
		void freeCommandBuffers();


		LveWindow& lveWindow;
		LveDevice& lveDevice;

		std::unique_ptr < LveSwapChain> lveSwapcChain;
		std::vector<VkCommandBuffer> commandBuffer;

		uint32_t curretImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};
}
