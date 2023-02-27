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
				glm::vec3 dir = (glm::vec3{ flock.boids[i].velocity.x, flock.boids[i].velocity.y, flock.boids[i].velocity.z });
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

	float App::getAngle(glm::vec2 a, glm::vec2 b) {
		float dot = a.x * b.x + a.y * b.y;//      # dot product between[x1, y1] and [x2, y2]
		float det = a.x * b.y - a.y * b.x;//      # determinant
		return atan2(det, dot);// # atan2(y, x) or atan2(sin, cos)
	}

	// temporary helper function, creates a 1x1x1 cube centered at offset
	std::unique_ptr<LveModel> App::createCubeModel(LveDevice& device, glm::vec3 offset) {
		std::vector<LveModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<LveModel>(device, vertices);
	}

	std::unique_ptr<LveModel> App::createConeModel(LveDevice& device, glm::vec3 offset, float radius) {
		std::vector<LveModel::Vertex> vertices;


		float circleRes = 4;
		float heightRes = 1;
		float theta = 3.1415926 * 2 / float(circleRes);
		float tangetial_factor = tanf(theta);//calculate the tangential factor 

		float radial_factor = cosf(theta);//calculate the radial factor
		float x = radius;//we start at angle = 0 

		float y = 0;

		for (int i = 0; i <= circleRes; i++)
		{

			glm::vec3 color = { ((double)rand() / (RAND_MAX)) ,((double)rand() / (RAND_MAX)),((double)rand() / (RAND_MAX)) };

			vertices.push_back({ { x, 0.0f, y },color });
			vertices.push_back({ {0,1,0}, color });
			float tx = -y;
			float ty = x;

			x += tx * tangetial_factor;
			y += ty * tangetial_factor;


			x *= radial_factor;
			y *= radial_factor;
			vertices.push_back({ { x, 0.0f, y },color });

		}

		x = radius;//we start at angle = 0 

		y = 0;

		for (int ii = 0; ii < circleRes; ii++)
		{
			vertices.push_back({ {0,0,0},{0,0,0} });
			vertices.push_back({ {x,0,y},{0,0,0} });


			float tx = -y;
			float ty = x;

			x += tx * tangetial_factor;
			y += ty * tangetial_factor;

			x *= radial_factor;
			y *= radial_factor;

			vertices.push_back({ {x,0,y},{0,0,0} });

		}

		return std::make_unique<LveModel>(device, vertices);
	}

	void App::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel = createConeModel(lveDevice, glm::vec3(0.0), 1.0f);

		for (int i = 0; i < numberOfBoids; i++) {
			auto obj = GameObject::createGameObject();
			obj.model = lveModel;
			obj.transform.translate = { 0,0,0 };
			obj.transform.scale = { 0.01f,0.01f,0.01f };
		//	obj.transform.rotation = { 0,glm::pi<float>(),0 };
			gameObjects.push_back(std::move(obj));
		}


	}


}