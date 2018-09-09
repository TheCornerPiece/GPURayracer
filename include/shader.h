#ifndef SHADER_H 
#define SHADER_H 

#include <GLFW/glfw3.h>


GLuint compileShader(const char *string, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	
	glShaderSource(shader, 1, (const GLchar **) &string, NULL);
	glCompileShader(shader);
	
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	
	if (success == GL_FALSE)
	{
		printf("**********\n%s\n**********\n", string);
		
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		
		GLchar info[logSize];
		
		glGetShaderInfoLog(shader, logSize, NULL, info);
		
		printf("shader message: %s", info);
	}
	
	return shader;
}


Shader createShader(char *filename)
{
	Shader shader;
	shader.program = glCreateProgram();
	
	char filenameString[strlen(filename) + 5];
	
	sprintf(filenameString, "%s.vert", filename);
	
	struct FileInfo vertexInfo = loadFile(filenameString, 1);
	printf("\nCompiling %s ...\n", filenameString);
	
	shader.vertShader = compileShader(vertexInfo.buffer, GL_VERTEX_SHADER);
	free(vertexInfo.buffer);
	
	sprintf(filenameString, "%s.frag", filename);
	
	// char *sdfString;
	struct FileInfo sdfLibInfo = loadFile("SDF.glsl", 0);
	struct FileInfo fragInfo = loadFile(filenameString, 1);
	printf("\nCompiling %s ...\n", fragInfo.buffer);

	// printf("%d + %d\n", sdfLength, fragLength);

	char *fullFragString;
	fullFragString = (char*)malloc((sdfLibInfo.length + fragInfo.length + 1) * sizeof(fullFragString[0]));
	strcpy(fullFragString, sdfLibInfo.buffer);
	strcat(fullFragString, fragInfo.buffer);

	// printf("%s", fullFragString);

	shader.fragShader = compileShader(fullFragString, GL_FRAGMENT_SHADER);
	free(fragInfo.buffer);
	free(sdfLibInfo.buffer);
	free(fullFragString);
	
	glAttachShader(shader.program, shader.vertShader);
	glAttachShader(shader.program, shader.fragShader);
	
	glLinkProgram(shader.program);
	glValidateProgram(shader.program);
	
	glDetachShader(shader.program, shader.vertShader);
	glDetachShader(shader.program, shader.fragShader);
	
	return shader;
}

void destroyShader(Shader shader)
{
	glDeleteShader(shader.vertShader);
	glDeleteShader(shader.fragShader);
	glDeleteProgram(shader.program);
}

#endif