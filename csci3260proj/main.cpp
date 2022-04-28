/*
Student Information
Student ID: 1155126232, 1155127248
Student Name: Tsoi Chak Yu, Tang Kai Yiu
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Draw.h"

Shader shader;
Shader skyboxShader;

Texture universeSkybox;
// Spacecraft
glm::vec3 SpacecraftPos(0.0f);
float spacecraftAngle = glm::radians(90.0f);
float spacecraftSpeed = 50.0f;
Model spacecraftObj = loadOBJ("objects/spacecraft.obj");
Texture spacecraftTexture[2];
Model alienObj = loadOBJ("objects/alienpeople.obj");
Texture alienTexture;
// Vehicle
float vehicleAngle = 0.0f;
float vehicleRotate = 1.0f;
bool ChangeVehicleColor[]{
	false,
	false,
	false
};
Model vehicleObj = loadOBJ("objects/alienvehicle.obj");
Texture vehicleTexture[2];
// Planet
float planetAngle = 0.0f;
float planetRotate = 0.5f;
Model planetObj = loadOBJ("objects/planet.obj");
Texture planetTexture[2];// Rock
GLuint rockAmount = 200;
glm::mat4 rockModelMatrices[200];
float rockAngle = 0.0f;
float rockRotate = 0.1f;
float rockSelfAngle = 0.0f;
float rockSelfRotate = 1.0f;
Model rockObj = loadOBJ("objects/rock.obj");
Texture rockTexture;
//Chicken
bool ChickenCollision[]{
	false,
	false
};
Model chickenObj = loadOBJ("objects/chicken.obj");
Texture chickenTexture;
//Bread
bool BreadCollision = false;
Model breadObj = loadOBJ("objects/bread.obj");
Texture breadTexture;

GLuint vao[8];
GLuint vbo[8];
GLuint ebo[8];
GLuint* buffers[] = { vao, vbo, ebo };

// Camera
glm::vec3 eye(0.0f, 10.0f, -15.0f);
glm::vec3 center(0.0f, 0.0f, 15.0f);

// Time
float DeltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;

struct KeyboardController {
	bool UP = false;
	bool DOWN = false;
	bool LEFT = false;
	bool RIGHT = false;
	bool SPACE = false;
	float intensity = 1.0f;
} keyboardCtl;

struct MouseController {
	double oldx = 0.0, oldy = 0.0;
	float sensitivity = 0.002f;
} mouseCtl;

// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
void get_OpenGL_info()
{
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

void sendSkybox() {
	const char* universeFaces[] = {
		"textures/universe skybox/right.bmp",
		"textures/universe skybox/left.bmp",
		"textures/universe skybox/top.bmp",
		"textures/universe skybox/bottom.bmp",
		"textures/universe skybox/front.bmp",
		"textures/universe skybox/back.bmp"
	};
	universeSkybox.setupCubemap(universeFaces);
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vao[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

}

void sendSpacecraft() {
	spacecraftTexture[0].setupTexture("textures/spacecraftTexture.bmp");
	spacecraftTexture[1].setupTexture("textures/leisure_spacecraftTexture.bmp");
	Draw(buffers, 1, spacecraftObj);
}

void sendAlienpeople() {
	alienTexture.setupTexture("textures/alienTexture.bmp");
	Draw(buffers, 2, alienObj);
}

void sendVehicle() {
	vehicleTexture[0].setupTexture("textures/alienTexture.bmp");
	vehicleTexture[1].setupTexture("textures/colorful_alien_vehicleTexture.bmp");
	Draw(buffers, 3, vehicleObj);
}

void sendPlanet() {
	planetTexture[0].setupTexture("textures/planetTexture.bmp");
	planetTexture[1].setupTexture("textures/planetNormal.bmp");
	Draw(buffers, 4, planetObj);
	for (int i = 0; i < 10; i++) {
		std::cout << planetObj.vertices[i * planetObj.vertices.size() / 10].tangent.x << ' ' << planetObj.vertices[i * planetObj.vertices.size() / 10].tangent.y << ' ' << planetObj.vertices[i * planetObj.vertices.size() / 10].tangent.z << std::endl;
	}
}

void sendRock() {
	rockTexture.setupTexture("textures/rockTexture.bmp");
	Draw(buffers, 5, rockObj);
}

void sendChicken() {
	chickenTexture.setupTexture("textures/chickenTexture.bmp");
	Draw(buffers, 6, chickenObj);
}

void sendBread() {
	breadTexture.setupTexture("textures/breadTexture.bmp");
	Draw(buffers, 7, breadObj);
};
void sendDataToOpenGL()
{
	// Generate buffers
	glGenVertexArrays(8, vao);
	glGenBuffers(8, vbo);
	glGenBuffers(8, ebo);

	sendSkybox();
	sendSpacecraft();
	sendAlienpeople();
	sendVehicle();
	sendPlanet();
	sendRock();
	sendChicken();
	sendBread();
}

void initializedGL(void) //run only once
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();

	shader.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
	skyboxShader.setupShader("SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	CreateRockModel(rockModelMatrices, rockAmount);
}

bool CheckCollision(glm::vec3 v1, glm::vec3 v2) {
	glm::mat4 identity(1.0);
	
	glm::mat4 m1 = glm::translate(identity, v1);
	glm::mat4 m2 = glm::translate(identity, v2);

	if (glm::length(m2[3] - m1[3]) < 20)
		return true;
	else
		return false;
}

void setUpLight() {
	shader.use();


	// Point light 
		shader.setVec3("pointLight.position", glm::vec3(-50.0f, 0.0f, -100.0f));
		shader.setFloat("pointLight.intensity", keyboardCtl.intensity);
		shader.setFloat("pointLight.constant", 1.0f);
		shader.setFloat("pointLight.linear", 0.0f);
		shader.setFloat("pointLight.quadratic", 0.0f);
		shader.setVec3("pointLight.ambient", 0.1f, 0.1f, 0.1f);
		shader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
		shader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);

}

void paintGL(void)  //always run
{
	// Skybox
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	skyboxShader.use();
	setUpLight();

	// ModelMatrix
	glm::mat4 modelMatrix(1.0f);
	// viewMatrix
	glm::mat4 viewMatrix = glm::lookAt(eye, center, glm::vec3(0.0f, 1.0f, 0.0f));
	// projectionMatrix
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);
	// skyBoxMatrix
	glm::mat4 skyboxMatrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));
	skyboxShader.setMat4("skyboxMatrix", skyboxMatrix);

	// Skybox
	glBindVertexArray(vao[0]);
	universeSkybox.bindCubemap(0);
	skyboxShader.setInt("skybox", 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	shader.use();

	// EyeWorldMatrix
	glm::mat4 eyeWorldMatrix = projectionMatrix * viewMatrix;
	shader.setMat4("eyeWorldMatrix", eyeWorldMatrix);
	// eyePosition
	glm::vec3 eyePosition(eye);
	shader.setVec3("eyePosition", eyePosition);

	// Time
	float currentFrame = glfwGetTime();
	DeltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Spacecraft Movement
	float spacecraftTempSpeed = spacecraftSpeed * DeltaTime;
	// Difference w.r.t. front
	glm::vec2 FdiffTemp(glm::cos(spacecraftAngle), glm::sin(spacecraftAngle));
	glm::vec2 Fdiff(0.0f);
	// Difference w.r.t. left
	glm::vec2 LdiffTemp(glm::cos(spacecraftAngle - glm::radians(90.0f)), glm::sin(spacecraftAngle - glm::radians(90.0f)));
	glm::vec2 Ldiff(0.0f);
	if (keyboardCtl.UP)
		Fdiff += FdiffTemp;
	if (keyboardCtl.DOWN)
		Fdiff -= FdiffTemp;
	if (keyboardCtl.LEFT)
		Ldiff += LdiffTemp;
	if (keyboardCtl.RIGHT)
		Ldiff -= LdiffTemp;
	if (keyboardCtl.SPACE) {
		Fdiff *= 3;
		Ldiff *= 3;
	}
	// Totall Difference
	glm::vec2 FLdiff = Fdiff + Ldiff;
	// Normalize for moving diagonally
	if ((keyboardCtl.UP || keyboardCtl.DOWN) && (keyboardCtl.LEFT || keyboardCtl.RIGHT))
		FLdiff = glm::normalize(FLdiff);
	FLdiff *= spacecraftTempSpeed;
	SpacecraftPos.x += FLdiff.x;
	SpacecraftPos.z += FLdiff.y;
	eye.x += FLdiff.x;
	eye.z += FLdiff.y;
	center.x += FLdiff.x;
	center.z += FLdiff.y;

	// Spacecraft
	glBindVertexArray(vao[1]);
	if(ChickenCollision[0]==true&& ChickenCollision[1] == true && BreadCollision == true && ChangeVehicleColor[0] == true && ChangeVehicleColor[1] == true && ChangeVehicleColor[2] == true)
		spacecraftTexture[1].bind(0);
	else
		spacecraftTexture[0].bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setInt("normalMappingFlag", 0);
	shader.setFloat("material.shininess", 50.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), SpacecraftPos);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f) - spacecraftAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f, 0.01f, 0.01f));
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, spacecraftObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Alien Position
	glm::vec3 AlienPosition[]{
		glm::vec3(30.0f, 5.14f, 100.0f),
		glm::vec3(-30.0f, 5.14f, 200.0f),
		glm::vec3(30.0f, 5.14f, 300.0f)
	};

	// Paint Aliens
	glBindVertexArray(vao[2]);
	alienTexture.bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setInt("normalMappingFlag", 0);
	shader.setFloat("material.shininess", 20.0f);
	for (unsigned int i = 0; i < 3; i++)
	{
		modelMatrix = glm::translate(glm::mat4(1.0f), AlienPosition[i]);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));
		shader.setMat4("modelMatrix", modelMatrix);
		glDrawElements(GL_TRIANGLES, alienObj.indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	// Vehicle rotation
	vehicleAngle += vehicleRotate * DeltaTime;
	if (vehicleAngle > glm::radians(360.0f))
		vehicleAngle -= glm::radians(360.0f);

	// Vehicle Position
	glm::vec3 VehiclePosition[]{
		glm::vec3(30.0f, 0.0f, 100.0f),
		glm::vec3(-30.0f, 0.0f, 200.0f),
		glm::vec3(30.0f, 0.0f, 300.0f)
	};

	// Paint Vehicles
	glDisable(GL_CULL_FACE);
	glBindVertexArray(vao[3]);
	shader.setInt("material.myTexture", 0);
	shader.setInt("normalMappingFlag", 0);
	shader.setFloat("material.shininess", 50.0f);
	for (unsigned int i = 0; i < 3; i++)
	{
		if (CheckCollision(SpacecraftPos, VehiclePosition[i]))
			ChangeVehicleColor[i] = true;
		if(ChangeVehicleColor[i] == true)
			vehicleTexture[1].bind(0);
		else
			vehicleTexture[0].bind(0);
		modelMatrix = glm::translate(glm::mat4(1.0f), VehiclePosition[i]);
		modelMatrix = glm::rotate(modelMatrix, vehicleAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));
		shader.setMat4("modelMatrix", modelMatrix);
		glDrawElements(GL_TRIANGLES, vehicleObj.indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	// Planet rotation
	planetAngle += planetRotate * DeltaTime;
	if (planetAngle > glm::radians(360.0f))
		planetAngle -= glm::radians(360.0f);

	//  Planet
	glBindVertexArray(vao[4]);
	planetTexture[0].bind(0);
	shader.setInt("material.myTexture", 0);
	planetTexture[1].bind(1);
	shader.setInt("material.myNormalTexture", 1);
	shader.setInt("normalMappingFlag", 1);
	shader.setFloat("material.shininess", 20.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -15.0f, 500.0f));
	modelMatrix = glm::rotate(modelMatrix, planetAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(15.0f));
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, planetObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Rock rotation
	rockAngle += rockRotate * DeltaTime;
	if (rockAngle > glm::radians(360.0f))
		rockAngle -= glm::radians(360.0f);
	rockSelfAngle += rockSelfRotate * DeltaTime;
	if (rockSelfAngle > glm::radians(360.0f))
		rockSelfAngle -= glm::radians(360.0f);

	//  Rock
	glBindVertexArray(vao[5]);
	rockTexture.bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setInt("normalMappingFlag", 0);
	shader.setFloat("material.shininess", 10.0f);
	glm::mat4 modelMatrixTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 500.0f));
	modelMatrixTemp = glm::rotate(modelMatrixTemp, rockAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 selfRotateMatrix = glm::rotate(glm::mat4(1.0f), rockSelfAngle, glm::vec3(0.4f, 0.6f, 0.8f));
	for (GLuint i = 0; i < rockAmount; i++) {
		modelMatrix = modelMatrixTemp * rockModelMatrices[i] * selfRotateMatrix;
		shader.setMat4("modelMatrix", modelMatrix);
		glDrawElements(GL_TRIANGLES, rockObj.indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	// Chicken Position
	glm::vec3 ChickenPosition[]{
		glm::vec3(15.0f, 3.0f, 100.0f),
		glm::vec3(-15.0f, 3.0f, 200.0f),
	};

	// Paint Chickens
	glBindVertexArray(vao[6]);
	chickenTexture.bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setInt("normalMappingFlag", 0);
	shader.setFloat("material.shininess", 20.0f);
	for (unsigned int i = 0; i < 2; i++)
	{
		if(CheckCollision(SpacecraftPos, ChickenPosition[i]))
			ChickenCollision[i] = true;
		if(!ChickenCollision[i]){
		modelMatrix = glm::translate(glm::mat4(1.0f), ChickenPosition[i]);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f));//0.02f
		shader.setMat4("modelMatrix", modelMatrix);
		glDrawElements(GL_TRIANGLES, chickenObj.indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	glBindVertexArray(0);

	// Paint Bread
	if (CheckCollision(SpacecraftPos, glm::vec3(15.0f, 3.0f, 300.0f)))
		BreadCollision = true;
	if(!BreadCollision){
	glBindVertexArray(vao[7]);
	breadTexture.bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setInt("normalMappingFlag", 0);
	shader.setFloat("material.shininess", 20.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 3.0f, 300.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f));//0.02f
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, breadObj.indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	// Sets the cursor position 
	if (firstMouse) {
		mouseCtl.oldx = x;
		mouseCtl.oldy = y;
		firstMouse = false;
	}
	float rotationSpeed = mouseCtl.sensitivity * (mouseCtl.oldx - x);
	spacecraftAngle -= rotationSpeed;
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
	// Change rotation axis to y-axis
	eye -= SpacecraftPos;
	center -= SpacecraftPos;
	// Rotation
	eye = glm::vec3(rotationMatrix * glm::vec4(eye, 1.0f));
	center = glm::vec3(rotationMatrix * glm::vec4(center, 1.0f));
	// Move Backward
	eye += SpacecraftPos;
	center += SpacecraftPos;
	mouseCtl.oldx = x;
	mouseCtl.oldy = y;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Sets the Keyboard callback for the current window.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		keyboardCtl.UP = true;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		keyboardCtl.DOWN = true;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		keyboardCtl.LEFT = true;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		keyboardCtl.RIGHT = true;
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		keyboardCtl.SPACE = true;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		keyboardCtl.UP = false;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		keyboardCtl.DOWN = false;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
		keyboardCtl.LEFT = false;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
		keyboardCtl.RIGHT = false;
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		keyboardCtl.SPACE = false;
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		if (keyboardCtl.intensity < 100)
			keyboardCtl.intensity *= 1.5;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		if (keyboardCtl.intensity > 0.1)
			keyboardCtl.intensity /= 1.5;
	}
}


int main(int argc, char* argv[])
{
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 2", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                  //    
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Enable raw mouse motion
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	initializedGL();

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		paintGL();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
