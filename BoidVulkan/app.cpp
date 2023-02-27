#include "app.h"
#include "RenderSystem.h"
#include <stdexcept>
#include "glm/gtc/constants.hpp"
#include "flockingbird.hpp"
#include "KeyboardController.h"
#include <array>
#include "Camera.h"
#include <chrono>
#include <limits>
#pragma comment(lib, "shell32.lib")


namespace lve {

	struct pushConstants {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};



	App::App() {
		loadGameObjects();
	}

	App::~App() {

	}

	void App::run() {
		glm::vec3 bounds{ 1000,1000,1000 };
		flockingbird::FlockSimulationParameters flockSimParams{
				speedLimit,
				forceLimit,
				positionIncrementScalingFactor,
				avoidanceRadius,
				visionRange,
				separationWeight,
				alignmentWeight,
				cohesionWeight,
				bounds.x,
				bounds.y,
				bounds.z

		};
		flockingbird::Flock flock{ numberOfBoids,bounds.x,bounds.y,bounds.z };
		flockingbird::FlockSimulation flockSim{ flockSimParams,flock,defaultRules };


		RenderSystem renderSys{ lveDevice,renderer.getSwapChainRenderPass() };
		Camera camera{};
		camera.setViewTarget(glm::vec3{ 0.0 }, glm::vec3{ 0,0,2.5 });

		auto viewerObj = GameObject::createGameObject();
		keyboardMovementController cameraController{};
		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			//	frameTime = glm::min(frameTime, 16.0f);


				//Camera
			cameraController.moveInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObj);
			camera.setViewYXZ(viewerObj.transform.translate, viewerObj.transform.rotation);
			float aspect = renderer.getAspectRatio();
			camera.setPerspective(glm::radians(50.0f), aspect, 0.1, 10);

			//Boids
			flockSim.step(frameTime);


			for (int i = 0; i < numberOfBoids; i++) {

				gameObjects[i].transform.translate.x = glm::clamp(flock.boids[i].position.x / bounds.x, -1.0f, 1.0f);
				gameObjects[i].transform.translate.y = glm::clamp(flock.boids[i].position.y / bounds.y, -1.0f, 1.0f);
				gameObjects[i].transform.translate.z = glm::clamp(flock.boids[i].position.z / bounds.z, -1.0f, 1.0f);
				glm::vec3 dir = -(glm::vec3{ flock.boids[i].velocity.x, flock.boids[i].velocity.y, flock.boids[i].velocity.z });
				gameObjects[i].transform.lookatMatrix =glm::inverse(glm::lookAt(gameObjects[i].transform.translate, dir + gameObjects[i].transform.translate, {0,-1,0}));
				
			}

			if (auto commandBuffer = renderer.beginFrame()) {

				// begin offscreen shadow pass
				// render shadow casting objects
				// end of oddscreen objects

				renderer.beginSwapChainRenderPass(commandBuffer);
				renderSys.renderGameObjects(commandBuffer, gameObjects, camera);
				renderer.endSwapChainRenderPass(commandBuffer);

				renderer.EndFrame();
			}

		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	void App::loadGameObjects() {
		//std::shared_ptr<LveModel> lveModel = createConeModel(lveDevice, glm::vec3(0.0), 1.0f);
		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "Models/paperplane.obj");
		for (int i = 0; i < numberOfBoids; i++) {
			auto obj = GameObject::createGameObject();
			obj.model = lveModel;
			obj.transform.translate = { 0,0,0 };
			obj.transform.scale = { 0.001f,0.001f,0.001f };
			//obj.transform.rotation = { glm::pi<float>(),0,0 };
			gameObjects.push_back(std::move(obj));
		}


	}


}