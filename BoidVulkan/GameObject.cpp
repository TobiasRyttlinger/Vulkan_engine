#include "GameObject.h"
namespace lve{


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

}

