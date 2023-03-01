#pragma once

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include "lve_model.h"
#include<memory>
#include <algorithm>
#include <functional>
namespace lve {

	struct transformComponent {
		glm::vec3 translate{};
		glm::vec3 scale{ 1.0f,1.0f,1.0f };
		glm::vec3 rotation;
		glm::mat4 lookatMatrix{1.0f};

		//Mat is equal to transl * rY *Rx*Rz*Scale
  // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
  // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
  // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class GameObject {
	public:
		using id_t = unsigned int;
		bool boid = false;
		static GameObject createGameObject() {
			static id_t currentId = 0;
			return GameObject(currentId++);
		}

		GameObject(const GameObject&) = delete;
		GameObject& operator= (const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator= (GameObject&&) = default;

		glm::mat4 LookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up);

		id_t getId() { return id; }
		std::shared_ptr<LveModel> model{};
		glm::vec3 color{};
		transformComponent transform{};

	private:
		GameObject(id_t objId) : id{ objId } {};


		id_t id;
	};


}