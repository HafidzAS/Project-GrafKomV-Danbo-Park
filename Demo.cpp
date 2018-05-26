#include "Demo.h"



Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);

	BuildCube();
	BuildFace();
	BuildBush();
	BuildTree();
	BuildLeaf();
	BuildChair();
	BuildPlane();
	BuildSky();
	InitCamera();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth / 2;
	double midY = screenHeight / 2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 1000;
	angleZ = (float)((midY - mouseY)) / 1000;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);




}

void Demo::Update(double deltaTime) {
	angle += (float)((deltaTime * 1.5f) / 1000);
	move(lurus, belok);
}

void Demo::Render() {
	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	DrawCube();
	DrawFace();
	DrawBush();
	DrawTree();
	DrawLeaf();
	DrawChair();
	DrawPlane();
	DrawSky();

	glDisable(GL_DEPTH_TEST);
}

void Demo::BuildCube() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("danbo body.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// right
		0.75,  1.5,  0.6, 0, 0,  // 4
		0.75,  1.5, -0.6, 1, 0,  // 5
		0.75, 0.5, -0.6, 1, 1,  // 6
		0.75, 0.5,  0.6, 0, 1,  // 7

								// back
								-0.75, 0.5, -0.6, 0, 0, // 8 
								0.75,  0.5, -0.6, 1, 0, // 9
								0.75,   1.5, -0.6, 1, 1, // 10
								-0.75,  1.5, -0.6, 0, 1, // 11

														 // left
														 -0.75, 0.5, -0.6, 0, 0, // 12
														 -0.75, 0.5,  0.6, 1, 0, // 13
														 -0.75,  1.5,  0.6, 1, 1, // 14
														 -0.75,  1.5, -0.6, 0, 1, // 15

																				  // upper
																				  0.75, 1.5,  0.6, 0, 0,   // 16
																				  -0.75, 1.5,  0.6, 1, 0,  // 17
																				  -0.75, 1.5, -0.6, 1, 1,  // 18
																				  0.75, 1.5, -0.6, 0, 1,   // 19

																										   // bottom
																										   -0.75, 0.5, -0.6, 0, 0, // 20
																										   0.75, 0.5, -0.6, 1, 0,  // 21
																										   0.75, 0.5,  0.6, 1, 1,  // 22
																										   -0.75, 0.5,  0.6, 0, 1, // 23

																																   // BODY
																																   // front
																																   -0.4, -0.6, 0.4, 0, 0,  // 24
																																   0.4, -0.6, 0.4, 1, 0,   // 25
																																   0.4,  0.5, 0.4, 1, 1,   // 26
																																   -0.4,  0.5, 0.4, 0, 1,  // 27

																																						   // right
																																						   0.4,  0.5,  0.4, 0, 0,  // 28
																																						   0.4,  0.5, -0.4, 1, 0,  // 29
																																						   0.4, -0.6, -0.4, 1, 1,  // 30
																																						   0.4, -0.6,  0.4, 0, 1,  // 31

																																												   // back
																																												   -0.4, -0.6, -0.4, 0, 0, // 32
																																												   0.4,  -0.6, -0.4, 1, 0, // 33
																																												   0.4,   0.5, -0.4, 1, 1, // 34
																																												   -0.4,  0.5, -0.4, 0, 1, // 35

																																																		   // left
																																																		   -0.4, -0.6, -0.4, 0, 0, // 36
																																																		   -0.4, -0.6,  0.4, 1, 0, // 37
																																																		   -0.4,  0.5,  0.4, 1, 1, // 38
																																																		   -0.4,  0.5, -0.4, 0, 1, // 39

																																																								   // upper
																																																								   0.4, 0.5,  0.4, 0, 0,   // 40
																																																								   -0.4, 0.5,  0.4, 1, 0,  // 41
																																																								   -0.4, 0.5, -0.4, 1, 1,  // 42
																																																								   0.4, 0.5, -0.4, 0, 1,   // 43

																																																														   // bottom
																																																														   -0.4, -0.6, -0.4, 0, 0, // 44
																																																														   0.4, -0.6, -0.4, 1, 0,  // 45
																																																														   0.4, -0.6,  0.4, 1, 1,  // 46
																																																														   -0.4, -0.6,  0.4, 0, 1, // 47 

																																																																				   // RIGHT HAND
																																																																				   // front
																																																																				   0.4, -0.4, 0.2, 0, 0,  // 48
																																																																				   0.6, -0.4, 0.2, 1, 0,   // 49
																																																																				   0.6,  0.5, 0.2, 1, 1,   // 50
																																																																				   0.4,  0.5, 0.2, 0, 1,  // 51

																																																																										  // right
																																																																										  0.6,  0.5,  0.2, 0, 0,  // 52
																																																																										  0.6,  0.5, -0.2, 1, 0,  // 53
																																																																										  0.6, -0.4, -0.2, 1, 1,  // 54
																																																																										  0.6, -0.4,  0.2, 0, 1,  // 55

																																																																																  // back
																																																																																  0.4, -0.4, -0.2, 0, 0, // 56
																																																																																  0.6,  -0.4, -0.2, 1, 0, // 57
																																																																																  0.6,   0.5, -0.2, 1, 1, // 58
																																																																																  0.4,  0.5, -0.2, 0, 1, // 59

																																																																																						 // left
																																																																																						 0.4, -0.4, -0.2, 0, 0, // 60
																																																																																						 0.4, -0.4,  0.2, 1, 0, // 61
																																																																																						 0.4,  0.5,  0.2, 1, 1, // 62
																																																																																						 0.4,  0.5, -0.2, 0, 1, // 63

																																																																																												// upper
																																																																																												0.6, 0.5,  0.2, 0, 0,   // 64
																																																																																												0.4, 0.5,  0.2, 1, 0,  // 65
																																																																																												0.4, 0.5, -0.2, 1, 1,  // 66
																																																																																												0.6, 0.5, -0.2, 0, 1,   // 67

																																																																																																		// bottom
																																																																																																		0.4, -0.4, -0.2, 0, 0, // 68
																																																																																																		0.6, -0.4, -0.2, 1, 0,  // 69
																																																																																																		0.6, -0.4,  0.2, 1, 1,  // 70
																																																																																																		0.4, -0.4,  0.2, 0, 1, // 71

																																																																																																							   // LEFT HAND
																																																																																																							   // front
																																																																																																							   -0.6, -0.4, 0.2, 0, 0,  // 72
																																																																																																							   -0.4, -0.4, 0.2, 1, 0,   // 73
																																																																																																							   -0.4, 0.5, 0.2, 1, 1,   // 74
																																																																																																							   -0.6, 0.5, 0.2, 0, 1,  // 75

																																																																																																													  // right
																																																																																																													  -0.4, 0.5, 0.2, 0, 0,  // 76
																																																																																																													  -0.4, 0.5, -0.2, 1, 0,  // 77
																																																																																																													  -0.4, -0.4, -0.2, 1, 1,  // 78
																																																																																																													  -0.4, -0.4, 0.2, 0, 1,  // 79

																																																																																																																			  // back
																																																																																																																			  -0.6, -0.4, -0.2, 0, 0, // 80
																																																																																																																			  -0.4, -0.4, -0.2, 1, 0, // 81
																																																																																																																			  -0.4, 0.5, -0.2, 1, 1, // 82
																																																																																																																			  -0.6, 0.5, -0.2, 0, 1, // 83

																																																																																																																									 // left
																																																																																																																									 -0.6, -0.4, -0.2, 0, 0, // 84
																																																																																																																									 -0.6, -0.4, 0.2, 1, 0, // 85
																																																																																																																									 -0.6, 0.5, 0.2, 1, 1, // 86
																																																																																																																									 -0.6, 0.5, -0.2, 0, 1, // 87

																																																																																																																															// upper
																																																																																																																															-0.4, 0.5, 0.2, 0, 0,   // 88
																																																																																																																															-0.6, 0.5, 0.2, 1, 0,  // 89
																																																																																																																															-0.6, 0.5, -0.2, 1, 1,  // 90
																																																																																																																															-0.4, 0.5, -0.2, 0, 1,   // 91

																																																																																																																																					 // bottom
																																																																																																																																					 -0.6, -0.4, -0.2, 0, 0, // 92
																																																																																																																																					 -0.4, -0.4, -0.2, 1, 0,  // 93
																																																																																																																																					 -0.4, -0.4, 0.2, 1, 1,  // 94
																																																																																																																																					 -0.6, -0.4, 0.2, 0, 1, // 95

																																																																																																																																											// LEFT LEG
																																																																																																																																											// front
																																																																																																																																											-0.3, -1.4, 0.2, 0, 0,  // 96
																																																																																																																																											-0.05, -1.4, 0.2, 1, 0,   // 97
																																																																																																																																											-0.05, -0.6, 0.2, 1, 1,   // 98
																																																																																																																																											-0.3, -0.6, 0.2, 0, 1,  // 99

																																																																																																																																																	// right
																																																																																																																																																	-0.05, -0.6, 0.2, 0, 0,  // 100
																																																																																																																																																	-0.05, -0.6, -0.2, 1, 0,  // 101
																																																																																																																																																	-0.05, -1.4, -0.2, 1, 1,  // 102
																																																																																																																																																	-0.05, -1.4, 0.2, 0, 1,  // 103

																																																																																																																																																							 // back
																																																																																																																																																							 -0.3, -1.4, -0.2, 0, 0, // 104
																																																																																																																																																							 -0.05, -1.4, -0.2, 1, 0, // 105
																																																																																																																																																							 -0.05, -0.6, -0.2, 1, 1, // 106
																																																																																																																																																							 -0.3, -0.6, -0.2, 0, 1, // 107

																																																																																																																																																													 // left
																																																																																																																																																													 -0.3, -1.4, -0.2, 0, 0, // 108
																																																																																																																																																													 -0.3, -1.4, 0.2, 1, 0, // 109
																																																																																																																																																													 -0.3, -0.6, 0.2, 1, 1, // 110
																																																																																																																																																													 -0.3, -0.6, -0.2, 0, 1, // 111

																																																																																																																																																																			 // upper
																																																																																																																																																																			 -0.05, -0.6, 0.2, 0, 0,   // 112
																																																																																																																																																																			 -0.3, -0.6, 0.2, 1, 0,  // 113
																																																																																																																																																																			 -0.3, -0.6, -0.2, 1, 1,  // 114
																																																																																																																																																																			 -0.05, -0.6, -0.2, 0, 1,   // 115

																																																																																																																																																																										// bottom
																																																																																																																																																																										-0.3, -1.4, -0.2, 0, 0, // 116
																																																																																																																																																																										-0.05, -1.4, -0.2, 1, 0,  // 117
																																																																																																																																																																										-0.05, -1.4, 0.2, 1, 1,  // 118
																																																																																																																																																																										-0.3, -1.4, 0.2, 0, 1, // 119

																																																																																																																																																																															   // RIGHT LEG
																																																																																																																																																																															   // front
																																																																																																																																																																															   0.05, -1.4, 0.2, 0, 0,  // 120
																																																																																																																																																																															   0.3, -1.4, 0.2, 1, 0,   // 121
																																																																																																																																																																															   0.3, -0.6, 0.2, 1, 1,   // 122
																																																																																																																																																																															   0.05, -0.6, 0.2, 0, 1,  // 123

																																																																																																																																																																																					   // right
																																																																																																																																																																																					   0.3, -0.6, 0.2, 0, 0,  // 124
																																																																																																																																																																																					   0.3, -0.6, -0.2, 1, 0,  // 125
																																																																																																																																																																																					   0.3, -1.4, -0.2, 1, 1,  // 126
																																																																																																																																																																																					   0.3, -1.4, 0.2, 0, 1,  // 127

																																																																																																																																																																																											  // back
																																																																																																																																																																																											  0.05, -1.4, -0.2, 0, 0, // 128
																																																																																																																																																																																											  0.3, -1.4, -0.2, 1, 0, // 129
																																																																																																																																																																																											  0.3, -0.6, -0.2, 1, 1, // 130
																																																																																																																																																																																											  0.05, -0.6, -0.2, 0, 1, // 131

																																																																																																																																																																																																	  // left
																																																																																																																																																																																																	  0.05, -1.4, -0.2, 0, 0, // 132
																																																																																																																																																																																																	  0.05, -1.4, 0.2, 1, 0, // 133
																																																																																																																																																																																																	  0.05, -0.6, 0.2, 1, 1, // 134
																																																																																																																																																																																																	  0.05, -0.6, -0.2, 0, 1, // 135

																																																																																																																																																																																																							  // upper
																																																																																																																																																																																																							  0.3, -0.6, 0.2, 0, 0,   // 136
																																																																																																																																																																																																							  0.05, -0.6, 0.2, 1, 0,  // 137
																																																																																																																																																																																																							  0.05, -0.6, -0.2, 1, 1,  // 138
																																																																																																																																																																																																							  0.3, -0.6, -0.2, 0, 1,   // 139

																																																																																																																																																																																																													   // bottom
																																																																																																																																																																																																													   0.05, -1.4, -0.2, 0, 0, // 140
																																																																																																																																																																																																													   0.3, -1.4, -0.2, 1, 0,  // 141
																																																																																																																																																																																																													   0.3, -1.4, 0.2, 1, 1,  // 142
																																																																																																																																																																																																													   0.05, -1.4, 0.2, 0, 1, // 143
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,  // front
		4,  5,  6,  4,  6,  7,  // right
		8,  9,  10, 8,  10, 11, // back
		12, 14, 13, 12, 15, 14, // left
		16, 18, 17, 16, 19, 18, // upper
		20, 22, 21, 20, 23, 22, // bottom 

		24, 25, 26, 24, 26, 27, // front
		28, 29, 30, 28, 30, 31, // right
		32, 33, 34, 32, 34, 35, // back
		36, 38, 37, 36, 39, 38, // left
		40, 42, 41, 40, 43, 42, // upper
		44, 46, 45, 44, 47, 46, // bottom

		48, 49, 50, 48, 50, 51, // front
		52, 53, 54, 52, 54, 55, // right
		56, 57, 58, 56, 58, 59, // back
		60, 62, 61, 60, 63, 62, // left
		64, 66, 65, 64, 67, 66, // upper
		68, 70, 69, 68, 71, 70, // bottom

		72, 73, 74, 72, 74, 75, // front
		76, 77, 78, 76, 79, 78, // right
		80, 81, 82, 80, 82, 83, // back
		84, 86, 85, 84, 87, 86, // left
		88, 90, 89, 88, 91, 90, // upper
		92, 94, 93, 92, 95, 94, // bottom

		96, 97, 98, 96, 98, 99,			// front
		100, 101, 102, 100, 103, 102,	// right
		104, 105, 106, 104, 106, 107,	// back
		108, 110, 109, 108, 111, 110,	// left
		112, 114, 113, 112, 115, 114,	// upper
		116, 118, 117, 116, 119, 118,	// bottom

		120,121,122,120,122,123,
		124,125,126,124,126,127,
		128,129,130,128,130,131,
		132,133,134,132,134,135,
		136,137,138,136,138,139,
		140,141,132,140,142,143,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
void Demo::BuildFace() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("danbo.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-0.75, 0.5, 0.6, 0, 0,  // 0
		0.75, 0.5, 0.6, 1, 0,   // 1
		0.75,  1.5, 0.6, 1, 1,   // 2
		-0.75,  1.5, 0.6, 0, 1,  // 3
	};
	unsigned int indices[] = { 0,  1,  2,  0,  2,  3,  /* front*/ };

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glGenBuffers(1, &EBO4);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
void Demo::BuildBush() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("bush.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		//1
		// front
		-5, -1.5, 8, 0, 0,  // 0
		-3, -1.5, 8, 1, 0,   // 1
		-3, 2, 8, 1, 1,   // 2
		-5, 2, 8, 0, 1,  // 3

						 // right
						 -3,  2,  8, 0, 0,  // 4
						 -3,  2,  6, 1, 0,  // 5
						 -3, -1.5, 6, 1, 1,  // 6
						 -3, -1.5,  8, 0, 1,  // 7

											  // back
											  -5, -1.5, 6, 0, 0, // 8 
											  -3, -1.5, 6, 1, 0, // 9
											  -3, 2, 6, 1, 1, // 10
											  -5, 2, 6, 0, 1, // 11

															  // left
															  -5, -1.5, 6, 0, 0, // 12
															  -5, -1.5, 8, 1, 0, // 13
															  -5, 2, 8, 1, 1, // 14
															  -5, 2, 6, 0, 1, // 15

																			  // upper
																			  -3, 2, 8, 0, 0,   // 16
																			  -5, 2,  8, 1, 0,  // 17
																			  -5, 2, 6, 1, 1,  // 18
																			  -3, 2, 6, 0, 1,   // 19

																								// bottom
																								-5, -1.5, 6, 0, 0, // 20
																								-3, -1.5, 6, 1, 0,  // 21
																								-3, -1.5,  8, 1, 1,  // 22
																								-5, -1.5,  8, 0, 1, // 23

																													// 2
																													// front
																													-5, -1.5, 5, 0, 0,  // 24
																													-3, -1.5, 5, 1, 0,   // 25
																													-3, 2, 5, 1, 1,   // 26
																													-5, 2, 5, 0, 1,  // 27

																																	 // right
																																	 -3,  2,  5, 0, 0,  // 28
																																	 -3,  2,  3, 1, 0,  // 29
																																	 -3, -1.5, 3, 1, 1,  // 30
																																	 -3, -1.5,  5, 0, 1,  // 31

																																						  // back
																																						  -5, -1.5, 3, 0, 0, // 32 
																																						  -3, -1.5, 3, 1, 0, // 33
																																						  -3,  2, 3, 1, 1, // 34
																																						  -5,  2, 3, 0, 1, // 35

																																										   // left
																																										   -5, -1.5, 3, 0, 0, // 36
																																										   -5, -1.5, 5, 1, 0, // 37
																																										   -5, 2, 5, 1, 1, // 38
																																										   -5, 2, 3, 0, 1, // 39

																																														   // upper
																																														   -3, 2, 5, 0, 0,   // 40
																																														   -5, 2,  5, 1, 0,  // 41
																																														   -5, 2, 3, 1, 1,  // 42
																																														   -3, 2, 3, 0, 1,   // 43

																																																			 // bottom
																																																			 -5, -1.5, 3, 0, 0, // 44
																																																			 -3, -1.5, 3, 1, 0,  // 45
																																																			 -3, -1.5,  5, 1, 1,  // 46
																																																			 -5, -1.5,  5, 0, 1, // 47	

																																																								 // 3
																																																								 // front
																																																								 -5, -1.5, 2, 0, 0,  // 24
																																																								 -3, -1.5, 2, 1, 0,   // 25
																																																								 -3, 2, 2, 1, 1,   // 26
																																																								 -5, 2, 2, 0, 1,  // 27

																																																												  // right
																																																												  -3,  2,  2, 0, 0,  // 28
																																																												  -3,  2,  0, 1, 0,  // 29
																																																												  -3, -1.5, 0, 1, 1,  // 30
																																																												  -3, -1.5,  2, 0, 1,  // 31

																																																																	   // back
																																																																	   -5, -1.5, 0, 0, 0, // 32 
																																																																	   -3, -1.5, 0, 1, 0, // 33
																																																																	   -3,  2, 0, 1, 1, // 34
																																																																	   -5,  2, 0, 0, 1, // 35

																																																																						// left
																																																																						-5, -1.5, 0, 0, 0, // 36
																																																																						-5, -1.5, 2, 1, 0, // 37
																																																																						-5, 2, 2, 1, 1, // 38
																																																																						-5, 2, 0, 0, 1, // 39

																																																																										// upper
																																																																										-3, 2, 2, 0, 0,   // 40
																																																																										-5, 2, 2, 1, 0,  // 41
																																																																										-5, 2, 0, 1, 1,  // 42
																																																																										-3, 2, 0, 0, 1,  // 43

																																																																														 // bottom
																																																																														 -5, -1.5, 0, 0, 0, // 44
																																																																														 -3, -1.5, 0, 1, 0, // 45
																																																																														 -3, -1.5, 2, 1, 1, // 46
																																																																														 -5, -1.5, 2, 0, 1, // 47

																																																																																			//4
																																																																																			// front
																																																																																			5, -1.5, 8, 0, 0,  // 0
																																																																																			3, -1.5, 8, 1, 0,   // 1
																																																																																			3, 2, 8, 1, 1,   // 2
																																																																																			5, 2, 8, 0, 1,  // 3

																																																																																							// right
																																																																																							3,	2, 8, 0, 0,  // 4
																																																																																							3, 2, 6, 1, 0,  // 5
																																																																																							3, -1.5, 6, 1, 1,  // 6
																																																																																							3, -1.5, 8, 0, 1,  // 7

																																																																																											   // back
																																																																																											   5, -1.5, 6, 0, 0, // 8 
																																																																																											   3, -1.5, 6, 1, 0, // 9
																																																																																											   3, 2, 6, 1, 1, // 10
																																																																																											   5, 2, 6, 0, 1, // 11

																																																																																															  // left
																																																																																															  5, -1.5, 6, 0, 0, // 12
																																																																																															  5, -1.5, 8, 1, 0, // 13
																																																																																															  5, 2, 8, 1, 1, // 14
																																																																																															  5, 2, 6, 0, 1, // 15

																																																																																																			 // upper
																																																																																																			 3, 2, 8, 0, 0,   // 16
																																																																																																			 5, 2, 8, 1, 0,  // 17
																																																																																																			 5, 2, 6, 1, 1,  // 18
																																																																																																			 3, 2, 6, 0, 1,   // 19

																																																																																																							  // bottom
																																																																																																							  5, -1.5, 6, 0, 0, // 20
																																																																																																							  3, -1.5, 6, 1, 0,  // 21
																																																																																																							  3, -1.5, 8, 1, 1,  // 22
																																																																																																							  5, -1.5, 8, 0, 1, // 23

																																																																																																												// 5
																																																																																																												// front
																																																																																																												5, -1.5, 5, 0, 0,  // 24
																																																																																																												3, -1.5, 5, 1, 0,   // 25
																																																																																																												3, 2, 5, 1, 1,   // 26
																																																																																																												5, 2, 5, 0, 1,  // 27

																																																																																																																// right
																																																																																																																3, 2, 5, 0, 0,  // 28
																																																																																																																3, 2, 3, 1, 0,  // 29
																																																																																																																3, -1.5, 3, 1, 1,  // 30
																																																																																																																3, -1.5, 5, 0, 1,  // 31

																																																																																																																				   // back
																																																																																																																				   5, -1.5, 3, 0, 0, // 32 
																																																																																																																				   3, -1.5, 3, 1, 0, // 33
																																																																																																																				   3, 2, 3, 1, 1, // 34
																																																																																																																				   5, 2, 3, 0, 1, // 35

																																																																																																																								  // left
																																																																																																																								  5, -1.5, 3, 0, 0, // 36
																																																																																																																								  5, -1.5, 5, 1, 0, // 37
																																																																																																																								  5, 2, 5, 1, 1, // 38
																																																																																																																								  5, 2, 3, 0, 1, // 39

																																																																																																																												 // upper
																																																																																																																												 3, 2, 5, 0, 0,   // 40
																																																																																																																												 5, 2, 5, 1, 0,  // 41
																																																																																																																												 5, 2, 3, 1, 1,  // 42
																																																																																																																												 3, 2, 3, 0, 1,   // 43

																																																																																																																																  // bottom
																																																																																																																																  5, -1.5, 3, 0, 0, // 44
																																																																																																																																  3, -1.5, 3, 1, 0,  // 45
																																																																																																																																  3, -1.5, 5, 1, 1,  // 46
																																																																																																																																  5, -1.5, 5, 0, 1, // 47	

																																																																																																																																					// 6
																																																																																																																																					// front
																																																																																																																																					5, -1.5, 2, 0, 0,  // 24
																																																																																																																																					3, -1.5, 2, 1, 0,  // 25
																																																																																																																																					3, 2, 2, 1, 1,   // 26
																																																																																																																																					5, 2, 2, 0, 1,  // 27

																																																																																																																																									// right
																																																																																																																																									3, 2, 2, 0, 0,  // 28
																																																																																																																																									3, 2, 0, 1, 0,  // 29
																																																																																																																																									3, -1.5, 0, 1, 1,  // 30
																																																																																																																																									3, -1.5, 2, 0, 1,  // 31

																																																																																																																																													   // back
																																																																																																																																													   5, -1.5, 0, 0, 0, // 32 
																																																																																																																																													   3, -1.5, 0, 1, 0, // 33
																																																																																																																																													   3, 2, 0, 1, 1, // 34
																																																																																																																																													   5, 2, 0, 0, 1, // 35

																																																																																																																																																	  // left
																																																																																																																																																	  5, -1.5, 0, 0, 0, // 36
																																																																																																																																																	  5, -1.5, 2, 1, 0, // 37
																																																																																																																																																	  5, 2, 2, 1, 1, // 38
																																																																																																																																																	  5, 2, 0, 0, 1, // 39

																																																																																																																																																					 // upper
																																																																																																																																																					 3, 2, 2, 0, 0,   // 40
																																																																																																																																																					 5, 2, 2, 1, 0,  // 41
																																																																																																																																																					 5, 2, 0, 1, 1,  // 42
																																																																																																																																																					 3, 2, 0, 0, 1,  // 43

																																																																																																																																																									 // bottom
																																																																																																																																																									 5, -1.5, 0, 0, 0, // 44
																																																																																																																																																									 3, -1.5, 0, 1, 0, // 45
																																																																																																																																																									 3, -1.5, 2, 1, 1, // 46
																																																																																																																																																									 5, -1.5, 2, 0, 1, // 47


																																																																																																																																																													   /*	 //7
																																																																																																																																																													   // front
																																																																																																																																																													   -0.8, 0, -5, 0, 0,  // 0
																																																																																																																																																													   1.3, 0, -5, 1, 0,   // 1
																																																																																																																																																													   1.3, 3, -5, 1, 1,   // 2
																																																																																																																																																													   -0.8, 3, -5, 0, 1,  // 3

																																																																																																																																																													   // right
																																																																																																																																																													   1.3, 3, -5, 0, 0,  // 4
																																																																																																																																																													   1.3, 3, -2.5, 1, 0,  // 5
																																																																																																																																																													   1.3, 0, -2.5, 1, 1,  // 6
																																																																																																																																																													   1.3, 0, -5, 0, 1,  // 7

																																																																																																																																																													   // back
																																																																																																																																																													   -0.8, 0, -2.5, 0, 0, // 8
																																																																																																																																																													   1.3, 0, -2.5, 1, 0, // 9
																																																																																																																																																													   1.3, 3, -2.5, 1, 1, // 10
																																																																																																																																																													   -0.8, 3, -2.5, 0, 1, // 11

																																																																																																																																																													   // left
																																																																																																																																																													   -0.8, 0, -2.5, 0, 0, // 12
																																																																																																																																																													   -0.8, 0, -5, 1, 0, // 13
																																																																																																																																																													   -0.8, 3, -5, 1, 1, // 14
																																																																																																																																																													   -0.8, 3, -2.5, 0, 1, // 15

																																																																																																																																																													   // upper
																																																																																																																																																													   1.3, 3, -5, 0, 0,   // 16
																																																																																																																																																													   -0.8, 3, -5, 1, 0,  // 17
																																																																																																																																																													   -0.8, 3, -2.5, 1, 1,  // 18
																																																																																																																																																													   1.3, 3, -2.5, 0, 1,   // 19

																																																																																																																																																													   // bottom
																																																																																																																																																													   -0.8, 0, -2.5, 0, 0, // 20
																																																																																																																																																													   1.3, 0, -2.5, 1, 0,  // 21
																																																																																																																																																													   1.3, 0, -5, 1, 1,  // 22
																																																																																																																																																													   -0.8, 0, -5, 0, 1, // 23
																																																																																																																																																													   */
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,  // front
		4,  5,  6,  4,  6,  7,  // right
		8,  9,  10, 8,  10, 11, // back
		12, 14, 13, 12, 15, 14, // left
		16, 18, 17, 16, 19, 18, // upper
		20, 22, 21, 20, 23, 22, // bottom 

		24, 25, 26, 24, 26, 27, // front
		28, 29, 30, 28, 30, 31, // right
		32, 33, 34, 32, 34, 35, // back
		36, 38, 37, 36, 39, 38, // left
		40, 42, 41, 40, 43, 42, // upper
		44, 46, 45, 44, 47, 46, // bottom

		48, 49, 50, 48, 50, 51, // front
		52, 53, 54, 52, 54, 55, // right
		56, 57, 58, 56, 58, 59, // back
		60, 62, 61, 60, 63, 62, // left
		64, 66, 65, 64, 67, 66, // upper
		68, 70, 69, 68, 71, 70, // bottom

		72, 73, 74, 72, 74, 75, // front
		76, 77, 78, 76, 79, 78, // right
		80, 81, 82, 80, 82, 83, // back
		84, 86, 85, 84, 87, 86, // left
		88, 90, 89, 88, 91, 90, // upper
		92, 94, 93, 92, 95, 94, // bottom

		96, 97, 98, 96, 98, 99,			// front
		100, 101, 102, 100, 103, 102,	// right
		104, 105, 106, 104, 106, 107,	// back
		108, 110, 109, 108, 111, 110,	// left
		112, 114, 113, 112, 115, 114,	// upper
		116, 118, 117, 116, 119, 118,	// bottom

		120,121,122,120,122,123,
		124,125,126,124,126,127,
		128,129,130,128,130,131,
		132,133,134,132,134,135,
		136,137,138,136,138,139,
		140,142,141,140,143,142,

		144,145,146,144,146,147,
		148,149,150,148,150,151,
		152,153,154,152,154,155,
		156,157,158,156,158,159,
		160,161,162,160,162,163,
		164,166,165,164,167,166
	};

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);

	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

}

void Demo::BuildTree() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture5);
	glBindTexture(GL_TEXTURE_2D, texture5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("crate.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		//7
		// front
		0, -1.5, -4, 0, 0,  // 0
		0.5, -1.5, -4, 1, 0,   // 1
		0.5, 3, -4, 1, 1,   // 2
		0, 3, -4, 0, 1,  // 3

						 // right
						 0.5, 3, -4, 0, 0,  // 4
						 0.5, 3, -3.5, 1, 0,  // 5
						 0.5, -1.5, -3.5, 1, 1,  // 6
						 0.5, -1.5, -4, 0, 1,  // 7

											   // back
											   0, -1.5, -3.5, 0, 0, // 8 
											   0.5, -1.5, -3.5, 1, 0, // 9
											   0.5, 3, -3.5, 1, 1, // 10
											   0, 3, -3.5, 0, 1, // 11

																 // left
																 0, -1.5, -3.5, 0, 0, // 12
																 0, -1.5, -4, 1, 0, // 13
																 0, 3, -4, 1, 1, // 14
																 0, 3, -3.5, 0, 1, // 15

																				   // upper
																				   0.5, 3, -4, 0, 0,   // 16
																				   0, 3, -4, 1, 0,  // 17
																				   0, 3, -3.5, 1, 1,  // 18
																				   0.5, 3, -3.5, 0, 1,   // 19

																										 // bottom
																										 0, -1.5, -3.5, 0, 0, // 20
																										 0.5, -1.5, -3.5, 1, 0,  // 21
																										 0.5, -1.5, -4, 1, 1,  // 22
																										 0, -1.5, -4, 0, 1, // 23
	};
	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,  // front
		4,  5,  6,  4,  6,  7,  // right
		8,  9,  10, 8,  10, 11, // back
		12, 14, 13, 12, 15, 14, // left
		16, 18, 17, 16, 19, 18, // upper
		20, 22, 21, 20, 23, 22, // bottom 
	};
	glGenVertexArrays(1, &VAO5);
	glGenBuffers(1, &VBO5);
	glGenBuffers(1, &EBO5);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO5);

	glBindBuffer(GL_ARRAY_BUFFER, VBO5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO5);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::BuildLeaf() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture6);
	glBindTexture(GL_TEXTURE_2D, texture6);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("leaf.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		//1
		// front
		-0.8, 0, -2.5, 0, 0,  // 0
		1.3, 0, -2.5, 1, 0,   // 1
		0.25, 3, -3.75, 1, 1,   // 2

								// right
								0.25, 3, -3.75, 0, 0,  // 3
								1.3, 0, -2.5, 1, 0,  // 4
								1.3, 0, -5, 1, 1,  // 5

												   // back
												   -0.8, 0, -5, 0, 0, // 6
												   1.3, 0, -5, 1, 0, // 7
												   0.25, 3, -3.75, 1, 1, // 8

																		 // left
																		 -0.8, 0, -2.5, 0, 0, // 9
																		 -0.8, 0, -5, 1, 0, // 10
																		 0.25, 3, -3.75, 1, 1, // 11

																							   // bottom
																							   -0.8, 0, -2.5, 0, 0, // 12
																							   1.3, 0, -2.5, 1, 0,  // 13
																							   1.3, 0, -5, 1, 1,  // 14
																							   -0.8, 0, -5, 0, 1, // 15


																												  //2
																												  // front
																												  -0.5, 1.5, -3, 0, 0,  // 16
																												  1, 1.5, -3, 1, 0,     // 17
																												  0.25, 4, -3.75, 1, 1, // 18

																																		// right
																																		0.25, 4, -3.75, 0, 0,// 19
																																		1, 1.5, -3, 1, 0,  // 20
																																		1, 1.5, -4.5, 1, 1,    // 21

																																							   // back
																																							   -0.5, 1.5, -4.5, 0, 0, // 22
																																							   1, 1.5, -4.5, 1, 0,  // 23
																																							   0.25, 4, -3.75, 1, 1,// 24

																																													// left
																																													-0.5, 1.5, -3, 0, 0, // 25
																																													-0.5, 1.5, -4.5, 1, 0,   // 26
																																													0.25, 4, -3.75, 1, 1,// 27

																																																		 // bottom
																																																		 -0.5, 1.5, -3, 0, 0, // 28
																																																		 1, 1.5, -3, 1, 0,  // 29
																																																		 1, 1.5, -4.5, 1, 1,  // 30
																																																		 -0.5, 1.5, -4.5, 0, 1, // 31

																																																								//3
																																																								// front
																																																								-0.2, 3, -3.25, 0, 0,    // 32
																																																								0.7, 3, -3.25, 1, 0,     // 3
																																																								0.25, 5, -3.75, 1, 1, // 34

																																																													  // right
																																																													  0.25, 5, -3.75, 0, 0,// 35
																																																													  0.7, 3, -3.25, 1, 0,  // 36
																																																													  0.7, 3, -4.25, 1, 1,    // 37

																																																																			  // back
																																																																			  -0.2, 3, -4.25, 0, 0, // 38
																																																																			  0.7, 3, -4.25, 1, 0,  // 39
																																																																			  0.25, 5, -3.75, 1, 1,// 40

																																																																								   // left
																																																																								   -0.2, 3, -3.25, 0, 0, // 41
																																																																								   -0.2, 3, -4.25, 1, 0,   // 42
																																																																								   0.25, 5, -3.75, 1, 1,// 43

																																																																														// bottom
																																																																														-0.2, 3, -3.25, 0, 0, // 44
																																																																														0.7, 3, -3.25, 1, 0,  // 45
																																																																														0.7, 3, -4.25, 1, 1,  // 46
																																																																														-0.2, 3, -4.25, 0, 1, // 47

	};
	unsigned int indices[] = {
		0,  2,  1,  // front
		3,  5,  4,  // right
		6,  8,  7,  // back
		9,  11, 10, // left
		12, 14, 13, 12, 15, 14, // bottom

		16,  18,  17,  // front
		19,  21,  20,  // right
		22,  24,  23,  // back
		25,  27, 26, // left
		28, 30, 29, 28, 31, 30, // bottom

		32,  34,  33,  // front
		35,  37,  36,  // right
		38,  40,  39,  // back
		41,  43, 42, // left
		44, 46, 45, 44, 47, 46, // bottom

	};
	glGenVertexArrays(1, &VAO6);
	glGenBuffers(1, &VBO6);
	glGenBuffers(1, &EBO6);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO6);

	glBindBuffer(GL_ARRAY_BUFFER, VBO6);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO6);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::BuildChair() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture7);
	glBindTexture(GL_TEXTURE_2D, texture7);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("chair.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		//1
		// front
		-3.8, -1.4, -4, 0, 0,  // 0
		-3.6, -1.4, -4, 1, 0,   // 1
		-3.6, -0.4, -4, 1, 1,   // 2
		-3.8, -0.4, -4, 0, 1,  // 3

							   // right
							   -3.6, -0.4, -4, 0, 0,  // 4
							   -3.6, -0.4, -3.5, 1, 0,  // 5
							   -3.6, -1.4, -3.5, 1, 1,  // 6
							   -3.6, -1.4, -4, 0, 1,  // 7

													  // back
													  -3.8, -1.4, -3.5, 0, 0, // 8 
													  -3.6, -1.4, -3.5, 1, 0, // 9
													  -3.6, -0.4, -3.5, 1, 1, // 10
													  -3.8, -0.4, -3.5, 0, 1, // 11

																			  // left
																			  -3.8, -1.4, -3.5, 0, 0, // 12
																			  -3.8, -1.4, -4, 1, 0, // 13
																			  -3.8, -0.4, -4, 1, 1, // 14
																			  -3.8, -0.4, -3.5, 0, 1, // 15

																									  // upper
																									  -3.6, -0.4, -4, 0, 0,   // 16
																									  -3.8, -0.4, -4, 1, 0,  // 17
																									  -3.8, -0.4, -3.5, 1, 1,  // 18
																									  -3.6, -0.4, -3.5, 0, 1,   // 19

																																// bottom
																																-3.8, -1.4, -3.5, 0, 0, // 20
																																-3.6, -1.4, -3.5, 1, 0,  // 21
																																-3.6, -1.4, -4, 1, 1,  // 22
																																-3.8, -1.4, -4, 0, 1, // 23

																																					  //2
																																					  // front
																																					  -1.9, -1.4, -4, 0, 0,  // 0
																																					  -1.7, -1.4, -4, 1, 0,   // 1
																																					  -1.7, -0.4, -4, 1, 1,   // 2
																																					  -1.9, -0.4, -4, 0, 1,  // 3

																																											 // right
																																											 -1.7, -0.4, -4, 0, 0,  // 4
																																											 -1.7, -0.4, -3.5, 1, 0,  // 5
																																											 -1.7, -1.4, -3.5, 1, 1,  // 6
																																											 -1.7, -1.4, -4, 0, 1,  // 7

																																																	// back
																																																	-1.9, -1.4, -3.5, 0, 0, // 8 
																																																	-1.7, -1.4, -3.5, 1, 0, // 9
																																																	-1.7, -0.4, -3.5, 1, 1, // 10
																																																	-1.9, -0.4, -3.5, 0, 1, // 11

																																																							// left
																																																							-1.9, -1.4, -3.5, 0, 0, // 12
																																																							-1.9, -1.4, -4, 1, 0, // 13
																																																							-1.9, -0.4, -4, 1, 1, // 14
																																																							-1.9, -0.4, -3.5, 0, 1, // 15

																																																													// upper
																																																													-1.7, -0.4, -4, 0, 0,   // 16
																																																													-1.9, -0.4, -4, 1, 0,  // 17
																																																													-1.9, -0.4, -3.5, 1, 1,  // 18
																																																													-1.7, -0.4, -3.5, 0, 1,   // 19

																																																																			  // bottom
																																																																			  -1.9, -1.4, -3.5, 0, 0, // 20
																																																																			  -1.7, -1.4, -3.5, 1, 0,  // 21
																																																																			  -1.7, -1.4, -4, 1, 1,  // 22
																																																																			  -1.9, -1.4, -4, 0, 1, // 23
																																																																			  -3.8, -1.4, -4, 0, 1, // 23
																																																																									//3
																																																																									// front
																																																																									-4, -0.2, -4, 0, 0,  // 0
																																																																									-1.5, -0.2, -4, 1, 0,   // 1
																																																																									-1.5, 1, -4, 1, 1,   // 2
																																																																									-4, 1, -4, 0, 1,  // 3

																																																																													  // right
																																																																													  -1.5, 1, -4, 0, 0,  // 4
																																																																													  -1.5, 1, -3.5, 1, 0,  // 5
																																																																													  -1.5, -0.2, -3.5, 1, 1,  // 6
																																																																													  -1.5, -0.2, -4, 0, 1,  // 7

																																																																																			 // back
																																																																																			 -4, -0.2, -3.5, 0, 0, // 8 
																																																																																			 -1.5, -0.2, -3.5, 1, 0, // 9
																																																																																			 -1.5, 1, -3.5, 1, 1, // 10
																																																																																			 -4, 1, -3.5, 0, 1, // 11

																																																																																								// left
																																																																																								-4, -0.2, -3.5, 0, 0, // 12
																																																																																								-4, -0.2, -4, 1, 0, // 13
																																																																																								-4, 1, -4, 1, 1, // 14
																																																																																								-4, 1, -3.5, 0, 1, // 15

																																																																																												   // upper
																																																																																												   -1.5, 1, -4, 0, 0,   // 16
																																																																																												   -4, 1, -4, 1, 0,  // 17
																																																																																												   -4, 1, -3.5, 1, 1,  // 18
																																																																																												   -1.5, 1, -3.5, 0, 1,   // 19

																																																																																																		  // bottom
																																																																																																		  -4, -0.2, -3.5, 0, 0, // 20
																																																																																																		  -1.5, -0.2, -3.5, 1, 0,  // 21
																																																																																																		  -1.5, -0.2, -4, 1, 1,  // 22
																																																																																																		  -4, -0.2, -4, 0, 1, // 23
	};
	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,  // front
		4,  5,  6,  4,  6,  7,  // right
		8,  9,  10, 8,  10, 11, // back
		12, 14, 13, 12, 15, 14, // left
		16, 18, 17, 16, 19, 18, // upper
		20, 22, 21, 20, 23, 22, // bottom 

		24, 25, 26, 24, 26, 27, // front
		28, 29, 30, 28, 30, 31, // right
		32, 33, 34, 32, 34, 35, // back
		36, 38, 37, 36, 39, 38, // left
		40, 42, 41, 40, 43, 42, // upper
		44, 46, 45, 44, 47, 46, // bottom

		48, 49, 50, 48, 50, 51, // front
		52, 53, 54, 52, 54, 55, // right
		56, 57, 58, 56, 58, 59, // back
		60, 62, 61, 60, 63, 62, // left
		64, 66, 65, 64, 67, 66, // upper
		68, 70, 69, 68, 71, 70, // bottom

	};
	glGenVertexArrays(1, &VAO7);
	glGenBuffers(1, &VBO7);
	glGenBuffers(1, &EBO7);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO7);

	glBindBuffer(GL_ARRAY_BUFFER, VBO7);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO7);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::BuildSky() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture8);
	glBindTexture(GL_TEXTURE_2D, texture8);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("skybox.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-100, -0.1, 100, 0.75, 0.25,  // 0
		100, -0.1, 100, 1, 0.25,   // 1
		100, 30, 100, 1, 0.5,   // 2
		-100, 30, 100, 0.75, 0.5,  // 3

								   // right
								   100, 30, -100, 0.5, 0.5,  // 4
								   100, 30, 100, 0.75, 0.5,  // 5
								   100, -0.1, 100, 0.75, 0.25,  // 6
								   100, -0.1, -100, 0.5, 0.25,  // 7

																// back
																-100, -0.1, -100, 0.25, 0.25, // 8 
																100, -0.1, -100, 0.5, 0.25, // 9
																100, 30, -100, 0.5, 0.5, // 10
																-100, 30, -100, 0.25, 0.5, // 11

																						   // left
																						   -100, -0.1, 100, 0, 0.25, // 12
																						   -100, -0.1, -100, 0.25, 0.25, // 13
																						   -100, 30, -100, 0.25, 0.5, // 14
																						   -100, 30, 100, 0, 0.5, // 15

																												  // upper
																												  100, 30, -100, 0.5, 0.5,   // 16
																												  -100, 30, -100, 0.25, 0.5,  // 17
																												  -100, 30, 100, 0.25, 0.75,  // 18
																												  100, 30, 100, 0.5, 0.75,   // 19

																																			 // bottom
																																			 -100, -0.1, 100, 0.25, 0, // 20
																																			 100, -0.1, 100, 0.5, 0,  // 21
																																			 100, -0.1, -100, 0.5, 0.25,  // 22
																																			 -100, -0.1, -100, 0.25, 0.25, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,  // front
		4,  5,  6,  4,  6,  7,  // right
		8,  9,  10, 8,  10, 11, // back
		12, 14, 13, 12, 15, 14, // left
		16, 18, 17, 16, 19, 18, // upper
		20, 22, 21, 20, 23, 22, // bottom 
	};

	glGenVertexArrays(1, &VAO8);
	glGenBuffers(1, &VBO8);
	glGenBuffers(1, &EBO8);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO8);

	glBindBuffer(GL_ARRAY_BUFFER, VBO8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO8);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawCube()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized



							//glm::mat4 model;
							//model = glm::translate(model, glm::vec3(0, 3, 0));

							////model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

							//model = glm::scale(model, glm::vec3(3, 3, 3));

							//GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
							//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

	glDrawElements(GL_TRIANGLES, 216, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawFace()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO4); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized



							 //glm::mat4 model;
							 //model = glm::translate(model, glm::vec3(0, 3, 0));

							 ////model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

							 //model = glm::scale(model, glm::vec3(3, 3, 3));

							 //GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
							 //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

	glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawBush()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 2);

	glBindVertexArray(VAO3); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

							 /*glm::mat4 model1;
							 GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model1");
							 glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
							 */
	glDrawElements(GL_TRIANGLES, 700, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawTree()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture5);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO5); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized



							 //glm::mat4 model;
							 //model = glm::translate(model, glm::vec3(0, 3, 0));

							 ////model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

							 //model = glm::scale(model, glm::vec3(3, 3, 3));

							 //GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
							 //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawLeaf()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture6);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 2);

	glBindVertexArray(VAO6); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

							 /*glm::mat4 model1;
							 GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model1");
							 glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
							 */
	glDrawElements(GL_TRIANGLES, 100, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawChair()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture7);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO7); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized



							 //glm::mat4 model;
							 //model = glm::translate(model, glm::vec3(0, 3, 0));

							 ////model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

							 //model = glm::scale(model, glm::vec3(3, 3, 3));

							 //GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
							 //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

	glDrawElements(GL_TRIANGLES, 300, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawSky()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture8);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO8); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized



							 //glm::mat4 model;
							 //model = glm::translate(model, glm::vec3(0, 3, 0));

							 ////model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

							 //model = glm::scale(model, glm::vec3(3, 3, 3));

							 //GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
							 //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

	glDrawElements(GL_TRIANGLES, 40, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::move(float lurus, float belok) {

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(belok, 1, lurus));

	//model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

	//model = glm::scale(model, glm::vec3(3, 3, 3));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void Demo::BuildPlane()
{
	// Load and create a texture 
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("brick.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// bottom
		-50.0, 0, -50.0,  0,  0,
		50.0, 0, -50.0, 50,  0,
		50.0, 0,  50.0, 50, 50,
		-50.0, 0,  50.0,  0, 50,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}



void Demo::DrawPlane()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}
void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 1.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.003f;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}

int main(int argc, char** argv) {
	RenderEngine &app = Demo();
	app.Start("Transformation: Transform Cube", 800, 600, false, false);
}