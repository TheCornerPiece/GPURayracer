#version 330 core

layout(location = 0) attribute vec3 inPosition;

uniform mat4 view;
uniform mat4 proj;
uniform float aspect;

out vec3 direction;
out vec3 camPos;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
	direction = (view * vec4(vec3(aspect, 1.0, -1.0) * inPosition, 0.0)).xyz;
	camPos = view[3].xyz;
}