#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "linmath.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "loader.h"
#include "mesh.h"
#include "shader.h"
#include "input.h"
#include "camera.h"

static mat4x4 model;
static mat4x4 tmpModel;
static mat4x4 view;
static mat4x4 proj;


struct Uniforms{
	int model;
	int view;
	int proj;
	int tex;
	int time;
};

struct Mesh{
	vec4 pos;
	vec3 size;
	quat rot;
	VertexArray *mesh;
	GLuint *tex;
};

void cube_init(struct Mesh *mesh, VertexArray *vao, GLuint *tex, vec3 pos, vec3 size, quat rot){
	mesh->pos[0] = pos[0];
	mesh->pos[1] = pos[1];
	mesh->pos[2] = pos[2];
	mesh->pos[3] = 1.0f;

	mesh->size[0] = size[0];
	mesh->size[1] = size[1];
	mesh->size[2] = size[2];
	
	vec3 axis = {rot[0], rot[1], rot[2]};
	quat_rotate(mesh->rot, rot[0], axis);

	mesh->mesh = vao;
	mesh->tex = tex;
}


void APIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
								   GLsizei length, const GLchar *message, const void *userParam)
{
	printf("%s\n", message);
}


static void window_size_callback(GLFWwindow* window, int w, int h) 
{ 
	struct Uniforms *uniforms = (struct Uniforms*) glfwGetWindowUserPointer(window);
	glViewport(0, 0, w, h);
	mat4x4_perspective(proj, .9f, (float)w / h, .1f, 500.0f);
	glUniformMatrix4fv(uniforms->proj, 1, GL_FALSE, (const GLfloat *)proj);
}


int main(){
	// WINDOW
	uint16_t width = 640;
	uint16_t height = 480;
	
	if (!glfwInit()){} // Initialization failed
	
	GLFWwindow *window = glfwCreateWindow(width, height, "Roblox", NULL, NULL);
	if (!window) {} // Window or OpenGL context creation failed
	
	glfwMakeContextCurrent(window);
	if(!gladLoadGL()) {
        printf("Something went wrong!\n");
        exit(-1);
    }
    //printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
	
	glfwSwapInterval(1);
	glfwSetWindowPos(window, 500, 300);
	
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, window_size_callback);
	
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	
	// OPENGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	
	if (0){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else{
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	
	glClearColor(.5, .5, .7, 1);
	
	// SHADER
	Shader simpleShader = createShader("shader");
	glUseProgram(simpleShader.program);
	
	struct Uniforms uniforms;
	uniforms.model = glGetUniformLocation(simpleShader.program, "model");
	uniforms.view = glGetUniformLocation(simpleShader.program, "view");
	uniforms.proj = glGetUniformLocation(simpleShader.program, "proj");
	uniforms.tex = glGetUniformLocation(simpleShader.program, "tex");
	uniforms.time = glGetUniformLocation(simpleShader.program, "time");
	glfwSetWindowUserPointer(window, &uniforms);
	
	window_size_callback(window, width, height);

	// TEXTURE
	GLuint texId = loadTexture("dirt.bmp");
	
	glActiveTexture(GL_TEXTURE0 + texId);
	glUniform1i(uniforms.tex, 0);
	
	// GAME

	MeshData meshData = objLoader("enemy.obj");
	VertexArray vertexArray = createMesh(&meshData);

	//VertexArray vertexArray = createCube();
	
	struct Mesh meshes[25];
	int meshCount = 0;

	vec3 pos = {0.0f, 0.0f, 0.0f};
	vec3 size = {1.0f, 4.0f, 1.0f};
	quat rot;
	quat_identity(rot);
	cube_init(&meshes[meshCount], &vertexArray, &texId, pos, size, rot);
	meshCount++;

	pos[0] = 0;
	pos[1] = -5;
	pos[2] = 0;
	size[0] = 100;
	size[1] = 1;
	size[2] = 100;
	quat_identity(rot);
	cube_init(&meshes[meshCount], &vertexArray, &texId, pos, size, rot);
	meshCount++;

	struct Mesh *player = &meshes[0];

	struct Camera camera;
	camera.pos[1] = 15;
	
	// UPDATE VARIABLES
	int axisCount;
	int buttonCount;
	const float* axes;
	const unsigned char* buttons;

	float dt;
	double lastFrame;
	double currentFrame;
	int frameCount = 0;

	struct Mesh *currentMesh;
	
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		dt = (float)(currentFrame - lastFrame);
		lastFrame = currentFrame;
		
		axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
		buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		/*
		printf("\n");
		for (int i=0; i < buttonCount; i++){
			printf("%i - %c\n", i, buttons[i]);
		}*/

		player->pos[0] += axes[0] * 25 * dt;
		player->pos[2] += -axes[1] * 25 * dt;
		
		mat4x4_identity(view);
		mat4x4_rotate(view, view, 1, 0, 0, 90.0f / 57.3f);
		mat4x4_translate_in_place(view, 0, -50, 0);
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, (const GLfloat *)view);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for (int i=0; i < meshCount; i++){
			currentMesh = &meshes[i];
			glBindVertexArray(currentMesh->mesh->vao);
			mat4x4_translate(model, currentMesh->pos[0], currentMesh->pos[1], currentMesh->pos[2]);
			mat4x4_rotate(model, model, currentMesh->rot[0], currentMesh->rot[1], currentMesh->rot[2], currentMesh->rot[3]);
			mat4x4_scale_aniso(model, model, currentMesh->size[0], currentMesh->size[1], currentMesh->size[2]);
			glUniformMatrix4fv(uniforms.model, 1, GL_FALSE, (const GLfloat *)model);
			glDrawElements(GL_TRIANGLES, currentMesh->mesh->numIndices, GL_UNSIGNED_INT, NULL);
		}
		
        glBindVertexArray(0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	destroyShader(simpleShader);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}