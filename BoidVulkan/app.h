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
		const bool twoD = false;
		float speedLimit = 5;
		float forceLimit = 3;
		float positionIncrementScalingFactor = 1;
		float avoidanceRadius = 50;
		float visionRange = 100;
	

		float separationWeight =  1.5;
		float alignmentWeight = 1.0;
		float cohesionWeight = 1.0;
		float avoidanceWeight = 3;
		int numberOfBoids = 100;


	private:
	
		void loadGameObjects();
		
		LveWindow lveWindow{ WIDTH,HEIGHT,"MyWindow" };
		LveDevice lveDevice{ lveWindow };
		Renderer renderer{ lveWindow,lveDevice };
		std::vector<GameObject> gameObjects;
	};
}
