#pragma once

#include "KeyboardController.h"
namespace lve {
	void keyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObj) {
		glm::vec3 rotate{ 0 };
		if (glfwGetKey(window, Keys.lookRight) == GLFW_PRESS) rotate.y += 1.0;
		if (glfwGetKey(window, Keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.0;

		if (glfwGetKey(window, Keys.lookUp) == GLFW_PRESS) rotate.x += 1.0;
		if (glfwGetKey(window, Keys.lookDown) == GLFW_PRESS) rotate.x -= 1.0;
	
		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			gameObj.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}
		gameObj.transform.rotation.x = glm::clamp(gameObj.transform.rotation.x, -1.5f, 1.5f);
		gameObj.transform.rotation.y = glm::mod(gameObj.transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObj.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw),0.0,cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z,0.0f,-forwardDir.x };
		const glm::vec3 upDir{ 0.0f,-1.0,0 };

		glm::vec3 moveDir{ 0.0f };

		if (glfwGetKey(window, Keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, Keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;

		if (glfwGetKey(window, Keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, Keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;

		if (glfwGetKey(window, Keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, Keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			gameObj.transform.translate += moveSpeed * dt * glm::normalize(moveDir);
		}
	}
}