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
		glm::mat4 mat4() {
		
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);

			return glm::mat4{
				{
					scale.x * (c2 * c3),
					scale.x * (c1 * s3 + c3*s1*s2),
					scale.x * (s1*s3 - c1*c3*s2),
					0.0f,
				},
				{
					scale.y * (-c2*s3),
					scale.y * (c1*c3 - s1*s2*s3),
					scale.y * (c3*s1 + c1*s2*s3),
					0.0f,
				},
				{
					scale.z * (s2),
					scale.z * (-c2*s1),
					scale.z * (c1*c2),
					0.0f,
				},
				{translate.x, translate.y, translate.z, 1.0f} } * lookatMatrix;
		}

	};

	class GameObject {
	public:
		using id_t = unsigned int;

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