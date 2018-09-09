#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

void key_down(int key){
	if (key == GLFW_KEY_W){
		
	}
	else if (key == GLFW_KEY_S){
		
	}
	else if (key == GLFW_KEY_A){
		printf("a");
	}
	else if (key == GLFW_KEY_D){
		printf("d");
	}
}

void key_up(int key){
	if (key == GLFW_KEY_W){
		printf("w");
	}
	else if (key == GLFW_KEY_S){
		printf("s");
	}
	else if (key == GLFW_KEY_A){
		printf("a");
	}
	else if (key == GLFW_KEY_D){
		printf("d");
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS){
		key_down(key);
	} else if (action == GLFW_RELEASE){
		key_up(key);
	}
}

#endif