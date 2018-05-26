#pragma once
#include "RenderEngine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <SOIL/SOIL.h>

class Demo :
	public RenderEngine
{
public:
	Demo();
	~Demo();
private:
	float viewCamX, viewCamY, viewCamZ, upCamX, upCamY, upCamZ, posCamX, posCamY, posCamZ, CAMERA_SPEED, fovy;
	GLuint shaderProgram, VBO, VAO, EBO, texture, VBO2, VAO2, EBO2, texture2, VBO3, VAO3, EBO3, texture3, VAO4, VBO4, EBO4, texture4, VAO5, VBO5, EBO5, texture5, VAO6, VBO6, EBO6, texture6, VAO7, VBO7, EBO7, texture7, VAO8, VBO8, EBO8, texture8;
	float speed;
	float angle = 0;
	float lurus=0, belok=0;
	virtual void Init();
	virtual void DeInit();
	virtual void Update(double deltaTime);
	virtual void Render();
	virtual void ProcessInput(GLFWwindow *window);
	void BuildSky();
	void BuildCube();
	void BuildFace();
	void BuildBush();
	void BuildTree();
	void BuildLeaf();
	void BuildChair();
	void BuildPlane();
	void DrawSky();
	void DrawBush();
	void DrawFace();
	void DrawTree();
	void DrawLeaf();
	void DrawChair();
	void DrawCube();
	void DrawPlane();
	//gerak
	void move(float lurus,float belok);
	void MoveCamera(float speed);
	void StrafeCamera(float speed);
	void RotateCamera(float speed);
	void InitCamera();
};

