#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Line.h"

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshlist;
std::vector<Line*> coordinateLines;
std::vector<Shader> objectShader;
std::vector<Shader> worldX_Shader;
std::vector<Shader> worldY_Shader;
std::vector<Shader> worldZ_Shader;

Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Object Vertex Shader 
static const char* vShader = "Shaders/object.vert";

// Object Fragment Shader
static const char* fShader = "Shaders/object.frag";

// World-X Fragment Shader
static const char* worldX_fShader = "Shaders/red.frag";

// World-Y Fragment Shader
static const char* worldY_fShader = "Shaders/green.frag";

// World-Z Fragment Shader
static const char* worldZ_fShader = "Shaders/blue.frag";

void CreateCoordinateLines() 
{
	GLfloat x_Vertices[] =
	{
		-3, 0, 0,
		3, 0, 0
	};

	Line* xLine = new Line();
	xLine->CreateLine(x_Vertices, 6);
	coordinateLines.push_back(xLine);

	GLfloat y_Vertices[] =
	{
		0, -3, 0,
		0, 3, 0
	};

	Line* yLine = new Line();
	yLine->CreateLine(y_Vertices, 6);
	coordinateLines.push_back(yLine);

	GLfloat z_Vertices[] =
	{
		0, 0, -3,
		0, 0, 3
	};

	Line* zLine = new Line();
	zLine->CreateLine(z_Vertices, 6);
	coordinateLines.push_back(zLine);


}

void CreateObject()
{
	unsigned int indices[] =
	{
		0, 1, 2,
		0, 2, 3,
		1, 0, 4,
		5, 1, 4,
		2, 1, 5,
		2, 5, 6,
		3, 2, 7,
		2, 6, 7,
		5, 4, 6,
		6, 4, 7,
		0, 3, 4,
		4, 3, 7
	};

	GLfloat vertices[] =
	{
		0, 1, 1,
		1, 1, 1,
		1, 1, 0,
		0, 1, 0,
		0, 0, 1,
		1, 0, 1,
		1, 0, 0,
		0, 0, 0
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 36, 36);
	meshlist.push_back(obj1);
}

void CreateShaders()
{
	Shader *s_Object = new Shader();
	s_Object->CreateFromFiles(vShader, fShader);
	objectShader.push_back(*s_Object);

	Shader *s_WorldX = new Shader();
	s_WorldX->CreateFromFiles(vShader, worldX_fShader);
	worldX_Shader.push_back(*s_WorldX);

	Shader *s_WorldY = new Shader();
	s_WorldY->CreateFromFiles(vShader, worldY_fShader);
	worldY_Shader.push_back(*s_WorldY);

	Shader *s_WorldZ = new Shader();
	s_WorldZ->CreateFromFiles(vShader, worldZ_fShader);
	worldZ_Shader.push_back(*s_WorldZ);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialize();

	CreateObject();
	CreateCoordinateLines();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 1.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 5.0f, 0.1f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight(), 0.1f, 100.0f);

	while (!mainWindow.GetShouldClose())
	{
		GLfloat now = glfwGetTime();  
		deltaTime = now - lastTime;  
		lastTime = now;

		glfwPollEvents();

		camera.KeyControl(mainWindow.GetKeys(), deltaTime);
		camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		uniformModel = worldY_Shader[0].GetModelLocation();
		uniformProjection = worldY_Shader[0].GetProjectionLocation();
		uniformView = worldY_Shader[0].GetViewLocation();

		glm::mat4 model(1.0f);

		worldX_Shader[0].UseShader();
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		coordinateLines[0]->RenderLine();

		worldY_Shader[0].UseShader();
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		coordinateLines[1]->RenderLine();

		worldZ_Shader[0].UseShader();
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		coordinateLines[2]->RenderLine();

		objectShader[0].UseShader();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		meshlist[0]->RenderMesh();

		glUseProgram(0);

		mainWindow.SwapBuffers();
	}

	return 0;
}