#pragma once

#include "camera.h"

#include <vulkan/vulkan.h>


namespace lve {
	struct frameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
	};



}