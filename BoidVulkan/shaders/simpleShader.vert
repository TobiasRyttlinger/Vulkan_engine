#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
	mat4 transform; //projection * view * modelMatrix
	mat4 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, 30000.0, -1.0));

void main() {

	vec3 ambientColor = vec3(0.02f);
	vec3 specularColor = vec3(1.0, 1.0, 1.0);

	gl_Position = push.transform * vec4(position, 1.0);

	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normals);

	float shininessVal = 80.0;
	float Ka = 1.0; //Ambient
	float Kd = 1.0; //Diffuse
	float Ks = 0.0; //Specular

  // Lambert's cosine law
	float lambertian = max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0.0);
	float specular = 0.0;

	if(lambertian > 0.0) {
		vec3 R = reflect(-DIRECTION_TO_LIGHT, normalWorldSpace);      // Reflected light vector
		vec3 V = normalize(-position); // Vector to viewer
    // Compute the specular term
		float specAngle = max(dot(R, V), 0.0);
		specular = pow(specAngle, shininessVal);
	}
	//fragColor = vec3(Ka * ambientColor + lambertian);
	fragColor =  vec3(Ka * ambientColor + Kd * lambertian * color + Ks * specular * specularColor);
}