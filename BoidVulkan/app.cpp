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
#include "lve_buffer.hpp"
#pragma comment(lib, "shell32.lib")


namespace lve {

	struct globalUbo {
		glm::mat4 projectionView{ 1.0f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.0f,-3.0f,-1.0f });
	};

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
		glm::vec3 bounds{ 300,300,300 };
		flockingbird::FlockSimulationParameters flockSimParams{
				speedLimit,
				forceLimit,
				positionIncrementScalingFactor,
				avoidanceRadius,
				visionRange,
				separationWeight,
				alignmentWeight,
				cohesionWeight,
				avoidanceWeight,
				twoD,
				bounds.x,
				bounds.y,
				bounds.z

		};
		flockingbird::Flock flock{ numberOfBoids,bounds.x,bounds.y,bounds.z };
		flockingbird::FlockSimulation flockSim{ flockSimParams,flock,defaultRules };

		LveBuffer globalUniformBuffer{
			lveDevice,
			sizeof(globalUbo),
			LveSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			lveDevice.properties.limits.minUniformBufferOffsetAlignment,
		};

		globalUniformBuffer.map();


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

			for (int i = 0; i < gameObjects.size(); i++) {
				if (gameObjects[i].boid) {

					gameObjects[i].transform.translate.x = glm::clamp(flock.boids[i].position.x / bounds.x, -1.0f, 1.0f);
					gameObjects[i].transform.translate.y = glm::clamp(flock.boids[i].position.y / bounds.y, -1.0f, 1.0f);
					gameObjects[i].transform.translate.z = glm::clamp(flock.boids[i].position.z / bounds.z, -1.0f, 1.0f);
					glm::vec3 dir = -(glm::vec3{ flock.boids[i].velocity.x, flock.boids[i].velocity.y, flock.boids[i].velocity.z });
					if (twoD) {
						dir.z = 0;
					}


					if (twoD) {
						//2D
						gameObjects[i].transform.lookatMatrix = glm::inverse(glm::lookAt(gameObjects[i].transform.translate, dir + gameObjects[i].transform.translate, { 0,0,-1 }));

					}
					else {
						//3D
						gameObjects[i].transform.lookatMatrix = glm::inverse(glm::lookAt(gameObjects[i].transform.translate, dir + gameObjects[i].transform.translate, { 0,-1,0 }));
					}
				}


			}

			if (auto commandBuffer = renderer.beginFrame()) {

				int frameIndex = renderer.getFrameIndex();


				frameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};



				// update
				globalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				globalUniformBuffer.writeToIndex(&ubo, frameIndex);
				globalUniformBuffer.flushIndex(frameIndex);




				// render

				renderer.beginSwapChainRenderPass(commandBuffer);
				renderSys.renderGameObjects(frameInfo, gameObjects);
				renderer.endSwapChainRenderPass(commandBuffer);

				renderer.EndFrame();
			}

		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	void App::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel{};
		if (!twoD) {
			lveModel = LveModel::createModelFromFile(lveDevice, "Models/PaperAirplane.obj");
			for (int i = 0; i < numberOfBoids; i++) {
				auto obj = GameObject::createGameObject();
				obj.model = lveModel;
				obj.boid = true;
				obj.transform.translate = { 0.0f,0.5f,2.5f };
				obj.transform.scale = glm::vec3(0.3/numberOfBoids);

				gameObjects.push_back(std::move(obj));
			}

		}
		else {
			lveModel = LveModel::createModelFromFile(lveDevice, "Models/triangle.obj");
			for (int i = 0; i < numberOfBoids; i++) {
				auto obj = GameObject::createGameObject();
				obj.model = lveModel;
				obj.boid = true;
				obj.transform.translate = { 0.0f,0.5f,2.5f };
				obj.transform.scale = glm::vec3(2.5/numberOfBoids);

				gameObjects.push_back(std::move(obj));
			}

		}

		//add bounding cube
		lveModel = LveModel::createModelFromFile(lveDevice, "Models/cube.obj");
		auto obj = GameObject::createGameObject();
		obj.model = lveModel;
		obj.boid = false;
		//obj.transform.translate = { 0.0f,0.5f,2.5f };
		//obj.transform.scale = { 1f,0.005f,0.005f };
		//obj.transform.scale = { 3,1.5,3 };
		//obj.transform.rotation = { 0,glm::pi<float>(),0 };
		//gameObjects.push_back(std::move(obj));
	}


}