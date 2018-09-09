#ifndef ECS_H
#define ECS_H

#include <GL/gl.h>
#include <stdio.h>
#include <stdint>
#include "linmath.h"
	
enum ComponentTypeEnum {
	COMPONENT_TYPE_INVALID = -1,
	COMPONENT_TYPE_TRANSFORM,
	COMPONENT_TYPE_MESH,
	COMPONENT_TYPE_MATERIAL,
	COMPONENT_TYPE_COLLIDER_SPHERE,
	COMPONENT_TYPE_COLLIDER_BOX,
	COMPONENT_TYPE_COLLIDER_MESH,
	COMPONENT_TYPE_AI,
	COMPONENT_COUNT
};

enum VertexAttributeEnum {
	VERTEX_ATTRIBUTE_POSITION,
	VERTEX_ATTRIBUTE_NORMAL,
	VERTEX_ATTRIBUTE_TEXCOORD,
	VERTEX_ATTRIBUTE_COLOR,
	VERTEX_ATTRIBUTE_INDEX,
	VERTEX_ATTRIBUTE_COUNT
};

enum ShaderStageEnum {
	SHADER_STAGE_TESSELATION,
	SHADER_STAGE_VERTEX,
	SHADER_STAGE_GEOMETRY,
	SHADER_STAGE_FRAGMENT,
	SHADER_STAGE_COUNT
};

typedef struct {
	size_t size;
	void *data;	
} Data;

typedef struct {
	int type;
	Data state;
} Component;

typedef struct {
	vec3 position, scale, rotation;
	mat4x4 model;
} Transform;

typedef struct {
	size_t vertexCount;
	vec3 *positions, *normals, *texcoords;
	size_t indexCount;
	unsigned int *indices;
} MeshData;

typedef struct {
	GLuint program;
	GLuint shaders[SHADER_STAGE_COUNT];
	const char* fileNames[SHADER_STAGE_COUNT];
} Shader;

typedef struct {
	Shader* shader;
} Material;

typedef struct {
	GLuint vao;
	GLuint vbo[VERTEX_ATTRIBUTE_COUNT];
	MeshData* data;
} Mesh;

#define DEFINE_COMPONENT(componentType, T)\
	Component create##T##Component(const T *data) {\
		Component component = {};\
		component.type = componentType;\
		size_t size = sizeof(T);\
		component.state.size = size;\
		component.state.data = malloc(size);\
		memcpy(component.state.data, data, size);\
		return component;\
	}

DEFINE_COMPONENT(COMPONENT_TYPE_TRANSFORM, Transform)
DEFINE_COMPONENT(COMPONENT_TYPE_MESH, Mesh)

void invalidateComponent(Component* component) {
	free(component->state.data);
	component->state.data = NULL;
	component->state.size = 0;
	component->type = COMPONENT_TYPE_INVALID;
}

typedef struct {
	uint32_t id;
	const char *name;
	uint8_t currentIndex;
	Component components[256];
} Entity;

typedef struct {
	int requiredComponents;
	Data state;

	void (*start)(Data *state);
	void (*update)(Data *state, Entity *entities, unsigned int entityCount);
	void (*end)(Data *state);
} System;

Entity createEntity(const char *name) { 
	Entity entity = {}; 
	Entity.name = malloc(strlen(name), strcpy(name));
	return entity; 
}

void addComponentToEntity(Entity *entity, const Component *component) { 
	memcpy(&entity->components[entity->currentIndex++], component, sizeof(Component)); 
}

#endif // ECS_H