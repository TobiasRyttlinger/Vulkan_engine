
#include "lve_model.h"
#include <cassert>
#include <cstring>
namespace lve {
	LveModel::LveModel(LveDevice &device, const std::vector<Vertex>& vertices) : lveDevice{ device } {
		createVertexBuffer(vertices);
	}

	LveModel::~LveModel() {
		vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
	}



	void LveModel::createVertexBuffer(const std::vector<Vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be 3");

		VkDeviceSize buffersize = sizeof(vertices[0]) * vertexCount;
		lveDevice.createBuffer(buffersize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer, vertexBufferMemory);

		void* data;
		vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, buffersize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(buffersize));
		vkUnmapMemory(lveDevice.device(), vertexBufferMemory);

	}

	void LveModel::draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}
	void LveModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffer[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffer, offsets);
	}

	std::vector<VkVertexInputBindingDescription> 	LveModel::Vertex::getBindingDescriptions() {
		std::vector< VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}
	std::vector<VkVertexInputAttributeDescription> 	LveModel::Vertex::getAttributeDescriptions() {
		std::vector< VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex,position);


		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex,color); //Calculate bite color
		return attributeDescriptions;
	}

}