#pragma once


#include "Lve_Window.hpp"
#include "my_engine_device.hpp"
#include "Renderer.h"

#include <memory>
#include<vector>
#include "GameObject.h"
#include "flockingbird.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

namespace lve {



	class App {
	public:
		static constexpr int WIDTH = 1280;
		static constexpr int HEIGHT = 720;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void run();

		float speedLimit = 5;
		float forceLimit = 3;
		float positionIncrementScalingFactor = 1;
		float avoidanceRadius = 40;
		float visionRange = 100;

		float separationWeight = 1.5;
		float alignmentWeight = 3;
		float cohesionWeight = 1.0;		
		int numberOfBoids = 1000;

		

	private:
	
		void loadGameObjects();
		
		LveWindow lveWindow{ WIDTH,HEIGHT,"MyWindow" };
		LveDevice lveDevice{ lveWindow };
		Renderer renderer{ lveWindow,lveDevice };
		std::unique_ptr<LveModel> createCubeModel(LveDevice& device, glm::vec3 offset);
		std::unique_ptr<LveModel> createConeModel(LveDevice& device, glm::vec3 offset, float radius);
		float getAngle(glm::vec2 a, glm::vec2 b);
		std::vector<GameObject> gameObjects;
	};
}
