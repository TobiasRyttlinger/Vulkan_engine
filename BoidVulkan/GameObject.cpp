#include "GameObject.h"
namespace lve {


	glm::mat4 GameObject::LookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
	{
		glm::vec3 zaxis = normalize(at - eye);
		glm::vec3 xaxis = normalize(cross(zaxis, up));
		glm::vec3 yaxis = cross(xaxis, zaxis);

		zaxis.z = 0;
		glm::normalize(zaxis);


		glm::mat4 viewMatrix = {
		  glm::vec4(xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eye)),
		  glm::vec4(yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eye)),
		  glm::vec4(zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eye)),
		  glm::vec4(0, 0, 0, 1)
		};

		return viewMatrix;
	}


	glm::mat4 transformComponent::mat4() {

		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);

		return glm::mat4{
			{
				scale.x * (c2 * c3),
				scale.x * (c1 * s3 + c3 * s1 * s2),
				scale.x * (s1 * s3 - c1 * c3 * s2),
				0.0f,
			},
			{
				scale.y * (-c2 * s3),
				scale.y * (c1 * c3 - s1 * s2 * s3),
				scale.y * (c3 * s1 + c1 * s2 * s3),
				0.0f,
			},
			{
				scale.z * (s2),
				scale.z * (-c2 * s1),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translate.x, translate.y, translate.z, 1.0f} } *lookatMatrix;
	}
	glm::mat3 transformComponent::normalMatrix() {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);

		const glm::vec3 inverseScale = 1.0f / scale;
		return glm::mat3{
			{
				inverseScale.x * (c2 * c3),
				inverseScale.x * (c1 * s3 + c3 * s1 * s2),
				inverseScale.x * (s1 * s3 - c1 * c3 * s2)

			},
			{
				inverseScale.y * (-c2 * s3),
				inverseScale.y * (c1 * c3 - s1 * s2 * s3),
				inverseScale.y * (c3 * s1 + c1 * s2 * s3)
			},
			{
				inverseScale.z * (s2),
				inverseScale.z * (-c2 * s1),
				inverseScale.z * (c1 * c2)

			}

		};

	}
}

