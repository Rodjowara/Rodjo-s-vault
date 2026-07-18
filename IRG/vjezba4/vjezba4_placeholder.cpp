// Local Headers

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 

//nekima mozda ne radi primjerASSIMP zbog ponovnih definicija stbi funkcija.
//Jedno od mogucih rjesenja je da se zakomentira linija #define STB_IMAGE_IMPLEMENTATION.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Standard Headers
#include <iostream>
#include <cstdlib>

#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "Object.h"

const int mWidth = 1280;
const int mHeight = 800;


int main(int argc, char* argv[]) {

	Assimp::Importer importer;

	std::string path(argv[0]);
	//std::string dirPath(path, 0, path.find_last_of("\\/"));
	//std::string resPath(dirPath);
	//resPath.append("\\resources"); //za linux pretvoriti u forwardslash
	//std::string objPath(resPath);
	//objPath.append("\\glava\\glava.obj"); //za linux pretvoriti u forwardslash|

	std::string objPath("C:/Users/ivrod/Documents/irg/irglab/build/primjerASSIMP/Debug/resources/glava/glava.obj");

	const aiScene* scene = importer.ReadFile(objPath.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FlipUVs |
		aiProcess_GenNormals

	);

	if (!scene) {
		std::cerr << importer.GetErrorString();
		return false;
	}

	Mesh* mymesh;
	Shader* shader;
	Object* object = nullptr;

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1280, 800, "App", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (scene->HasMeshes()) {

		aiMesh* mesh = scene->mMeshes[0];

		std::vector<glm::vec3> vertices;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			vertices.emplace_back(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			);
		}

		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices;j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		mymesh = new Mesh(vertices, indices);
		shader = new Shader("vertex.glsl", "fragment.glsl");

		glm::vec3 min, max;
		mymesh->getBoundingBox(min, max);

		glm::vec3 center = (min + max) * 0.5f;

		float dx = max.x - min.x;
		float dy = max.y - min.y;
		float dz = max.z - min.z;

		float m = std::max(dx, std::max(dy, dz));

		Transform transform = Transform();
		transform.move(-center);
		transform.setScale(glm::vec3(2/m, 2/m, 2/m));

		glm::mat4 modelMatrix = transform.getModelMatrix();
		mymesh->applyTransform(modelMatrix);

		Transform sceneTransform;
		object = new Object(mymesh, shader, sceneTransform);	
	}

	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(object) object->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}