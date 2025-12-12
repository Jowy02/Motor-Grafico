#pragma once
#include "Texture.h"
#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GameObject;

struct ModelMesh {
	GLuint VAO = 0, VBO = 0, EBO = 0;
	unsigned int indexCount = 0;
    Texture* texture = nullptr;

	std::vector<glm::vec3> positionsLocal;
	std::vector<glm::vec3> positionsWorld;
	std::vector<unsigned int> indices;
	std::vector<float> vertices;
};

class ComponentMesh 
{
public:
	ComponentMesh(const GameObject* owner);
	~ComponentMesh();
	std::vector<unsigned int> GetIndexCount() const { return mesh.indices; }
	std::vector<float> GetVertexCount() const { return mesh.vertices; }
	void RecreateBuffers();
	ModelMesh mesh;

	std::string filenameMesh;
	glm::vec3 minAABB;
	glm::vec3 maxAABB;
};
