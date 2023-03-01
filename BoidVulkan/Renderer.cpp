#include "Renderer.h"

#include <stdexcept>
#include <array>
namespace lve {

	struct pushConstants {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};



	Renderer::Renderer(LveWindow& lveWindow, LveDevice& lveDevice) : lveWindow{ lveWindow }, lveDevice{ lveDevice } {
		recreateSwapChain();
		createCommandBuffer();
	}

	Renderer::~Renderer() { freeCommandBuffers(); }


	void Renderer::createCommandBuffer() {
		commandBuffer.resize(lveSwapcChain->MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = lveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffer.size());

		if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffer.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffer!");
		}


	}


	void Renderer::freeCommandBuffers() {
		vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffer.size()), commandBuffer.data());
		commandBuffer.clear();
	}
	void Renderer::recreateSwapChain() {
		auto extent = lveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = lveWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(lveDevice.device());

		if (lveSwapcChain == nullptr) {
			lveSwapcChain = std::make_unique<LveSwapChain>(lveDevice, extent);

		}
		else {
			std::shared_ptr<LveSwapChain> oldSwapChain = std::move(lveSwapcChain);
			lveSwapcChain = std::make_unique<LveSwapChain>(lveDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormat(*lveSwapcChain.get())) {
				throw std::runtime_error("swapchain image or depth format has changed!");
			}
		}

		//createPipeline();
	}





	VkCommandBuffer Renderer::beginFrame() {
		assert(!isFrameStarted && "Cant call begin frame if already started!");


		auto result = lveSwapcChain->acquireNextImage(&curretImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to swap chain image");
		}

		isFrameStarted = true;
		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}
		return commandBuffer;
	}
	void Renderer::EndFrame() {
		assert(isFrameStarted && "Cant call end frame if not started!");

		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
		auto result = lveSwapcChain->submitCommandBuffers(&commandBuffer, &curretImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()) {
			lveWindow.resetWindowResizedFlag();
			recreateSwapChain();
			//return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to swap chain image");
		}
		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}
	void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cant call beginSwap if frame is in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cant call beginSwap if commandBuffer is from different frame!");
	

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = lveSwapcChain->getRenderPass();
		renderPassInfo.framebuffer = lveSwapcChain->getFrameBuffer(curretImageIndex);


		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = lveSwapcChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f,0.01f,0.01f,1.0f };
		//clearValues[0].color = { 1.0,1.0,1.0,1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(lveSwapcChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(lveSwapcChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0},lveSwapcChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}
	void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cant end beginSwap if frame is in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cant end beginSwap if commandBuffer is from different frame!");
		vkCmdEndRenderPass(commandBuffer);
	}


}