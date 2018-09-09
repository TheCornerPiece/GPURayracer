#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "linmath.h"
#include "stb_image.h"
#include "loader.h"
#include "mesh.h"
#include "shader.h"
#include "input.h"
#include "camera.h"
#include "game.h"
#include "scene.h"
#include "fbo.h"

static mat4x4 view;
static mat4x4 proj;

struct Uniforms{
	int view;
	int proj;
	int tex;
	int runTime;
	int aspect;
	int startingDist;
	int w, h;
};

void cube_init(struct Entity *entity, VertexArray *vao, GLuint *tex, vec3 pos, vec3 size, quat rot){
	entity->pos[0] = pos[0];
	entity->pos[1] = pos[1];
	entity->pos[2] = pos[2];
	entity->pos[3] = 1.0f;

	entity->size[0] = size[0];
	entity->size[1] = size[1];
	entity->size[2] = size[2];
	
	vec3 axis = {rot[0], rot[1], rot[2]};
	quat_rotate(entity->rot, rot[0], axis);

	entity->mesh = vao;
	entity->tex = tex;
}


void APIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
								   GLsizei length, const GLchar *message, const void *userParam)
{
	printf("%s\n", message);
}

static GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)
	

static GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		char *error;

		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM"; 
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE"; 
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION"; 
			break;
		#if 0
		case GL_STACK_OVERFLOW:                
			error = "STACK_OVERFLOW"; 
			break;
		case GL_STACK_UNDERFLOW:              
			error = "STACK_UNDERFLOW"; 
			break;
		#endif
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY"; 
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: 
			error = "INVALID_FRAMEBUFFER_OPERATION"; 
			break;
		}
		//LOG("OPENGL-ERROR:\n\t" << file << " (" << line << "):\n\t" << error << "\n");
	}
	return errorCode;
}


static void window_size_callback(GLFWwindow* window, int w, int h) 
{ 
	struct Uniforms *uniforms = (struct Uniforms*) glfwGetWindowUserPointer(window);
	glViewport(0, 0, w, h);
	float aspect = (float)w/h;
	glUniform1f(uniforms->aspect, aspect);
	mat4x4_perspective(proj, 80.0f / 57.3f, aspect, 0.1f, 100.0f);
	glUniformMatrix4fv(uniforms->proj, 1, GL_FALSE, (const GLfloat *)proj);
}


int main(){
	// WINDOW
	uint16_t width = 512;
	uint16_t height = 512;
	
	if (!glfwInit()){} // Initialization failed
	
	GLFWwindow *window = glfwCreateWindow(width, height, "", NULL, NULL);
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

	//glCheckError(glCheckError_);
	
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	
	// OPENGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	
	if (0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	
	glClearColor(.5, .5, .7, 1);
	
	// SHADER

	int fboSize = 128;
	int fboWidth = fboSize;
	int fboHeight = fboSize;
	GLuint fboId = loadFBO(fboWidth, fboHeight);

	Shader simpleShader = createShader("shader");
	glUseProgram(simpleShader.program);
	
	struct Uniforms uniforms;
	uniforms.view = glGetUniformLocation(simpleShader.program, "view");
	uniforms.tex = glGetUniformLocation(simpleShader.program, "tex");
	uniforms.runTime = glGetUniformLocation(simpleShader.program, "runTime");
	uniforms.aspect = glGetUniformLocation(simpleShader.program, "aspect");
	uniforms.startingDist = glGetUniformLocation(simpleShader.program, "startingDist");

	glfwSetWindowUserPointer(window, &uniforms);
	
	window_size_callback(window, width, height);

	// TEXTURE
	GLuint texId = loadTexture("dirt.bmp");
	
	glActiveTexture(GL_TEXTURE0 + texId);
	glUniform1i(uniforms.tex, 0);

	// VAOS

	VertexArray rect = createRect();
	glBindVertexArray(rect.vao);

	// UPDATE VARIABLES
	int axisCount;
	int buttonCount;
	const float* axes;
	const unsigned char* buttons;

	struct Mesh *currentMesh;

	double lastX, lastY;
	double mouseX, mouseY;

    unsigned int frames = 0;
    double counter = 1.0;
    
    double t = glfwGetTime();
    double lastTime, dt;

	vec3 camPos;
	camPos[0] = 0.0f;
	camPos[1] = 5.0f;
	camPos[2] = 0.0f;

	vec3 camVel;
	camVel[0] = 0.0f;
	camVel[1] = 0.0f;
	camVel[2] = 0.0f;

	float damping;
	float camYaw = 0.0f;
	float camPitch = 0.0f;
	float maxPitch = 60.0f / 57.3f;
	float playerHeight = 3.0f;
	float c, s;

	int grounded = 0;
	float moveSpeed = 50.0f;
	float jumpForce = 10.0f;
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        lastTime = t;
        t = glfwGetTime();
        dt = t - lastTime;
		if (dt > .05f) {
			dt = .05f;
		}

		#if 1
		counter -= dt;
		++frames;
		
		while(counter < 0.0) {
			// printf("%d\n", frames);
			counter += 1.0;
			frames = 0;
		}
		#endif

		glfwGetCursorPos(window, &mouseX, &mouseY);
		axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
		buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		c = cos(camYaw);
		s = sin(camYaw);

		float speed = moveSpeed * dt;

		if (buttons[0] && grounded == 1){
			camVel[1] += jumpForce;
			grounded = 0;
		}
		camVel[0] += (axes[0] * c + axes[1] * s) * speed;
		camVel[2] += (axes[1] * c - axes[0] * s) * speed;

		camVel[1] -= 10.0f * dt;
		damping = 1 - 10 * dt;
		camVel[0] *= damping;
		camVel[2] *= damping;

		camPos[0] += camVel[0] * dt;
		camPos[1] += camVel[1] * dt;
		camPos[2] += camVel[2] * dt;

		if (camPos[1] < playerHeight){
			camPos[1] = playerHeight;
			camVel[1] = 0.0f;
			grounded = 1;
		}

		camYaw += axes[2] * dt * 3.0;
		camPitch += axes[3] * -dt * 3.0;
		if (camPitch > maxPitch){
			camPitch = maxPitch;
		}
		else if (camPitch < -maxPitch){
			camPitch = -maxPitch;
		}
		
		mat4x4_identity(view);
		mat4x4_translate_in_place(view, camPos[0], camPos[1], camPos[2]);
		mat4x4_rotate(view, view, 0.0f, 1.0f, 0.0f, camYaw);
		mat4x4_rotate(view, view, 1.0f, 0.0f, 0.0f, camPitch);
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, (const GLfloat *)view);

		// glUniform1f(uniforms.startingDist, scene(camPos));
		glUniform1f(uniforms.startingDist, 0.0f);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUniform1f(uniforms.runTime, (float)t);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, rect.numIndices, GL_UNSIGNED_INT, NULL);

		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
		// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// glBlitFramebuffer(0, 0,
		// 					fboWidth, fboHeight,
		// 					0, 0,
		// 					uniforms.w, uniforms.h,
		// 					GL_COLOR_BUFFER_BIT,
		// 					GL_NEAREST);

		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glfwSwapBuffers(window);
	}
	
	destroyShader(simpleShader);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}