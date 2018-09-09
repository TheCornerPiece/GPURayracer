#ifndef MESH_H 
#define MESH_H 

#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

enum MeshAttributeEnum{
	MESH_ATTRIBUTE_POSITION,
	MESH_ATTRIBUTE_TEXCOORD,
	MESH_ATTRIBUTE_NORMAL,
	MESH_ATTRIBUTE_INDEX,
	MESH_ATTRIBUTE_COUNT
};

typedef struct
{
	GLuint vao;
	GLuint numIndices;
	GLuint vbo[3];
} VertexArray;


typedef struct 
{
	GLuint program;
	GLuint vertShader;
	GLuint fragShader;
} Shader;

typedef struct {
	size_t vertexCount;
	vec3 *vertices, *normals;
	vec2 *texcoords;
	size_t indexCount;
	unsigned int *indices;
} MeshData;


#define arraySize(x) (sizeof(x)/sizeof(x[0]))
#define createMeshData(data, v, vn, vt, vi) {\
	data.vertexCount = arraySize(v);\
	data.indexCount = arraySize(vi);\
	data.vertices = malloc(sizeof(v));\
	memcpy(data.vertices, v, sizeof(v));\
	data.normals = malloc(sizeof(vn));\
	memcpy(data.normals, vn, sizeof(vn));\
	data.texcoords = malloc(sizeof(vt));\
	memcpy(data.texcoords, vt, sizeof(vt));\
	data.indices = malloc(sizeof(vt));\
	memcpy(data.indices, vi, sizeof(vi));\
}\
	

void MeshData_destroy(MeshData *meshData)
{
	free(meshData->vertices);
	free(meshData->normals);
	free(meshData->texcoords);
	free(meshData->indices); 
}


VertexArray createMesh(MeshData *meshData)
{				  
	VertexArray vertexArray = {.numIndices = meshData->indexCount};
	
	glGenVertexArrays(1, &vertexArray.vao);
	glBindVertexArray(vertexArray.vao);
	
	glGenBuffers(MESH_ATTRIBUTE_COUNT, vertexArray.vbo);
	
	// POSITIONS
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray.vbo[MESH_ATTRIBUTE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * meshData->vertexCount, meshData->vertices, GL_STATIC_DRAW);
	
    glEnableVertexAttribArray(MESH_ATTRIBUTE_POSITION);
    glVertexAttribPointer(MESH_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	// INDICES
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArray.vbo[MESH_ATTRIBUTE_INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshData->indexCount, meshData->indices, GL_STATIC_DRAW);
    //glVertexAttribPointer(MESH_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	return vertexArray;
}


VertexArray createCube(){
	float vertices[6 * 4 * 3] = {-1.0f, -1.0f, -1.0f,
								 -1.0f, -1.0f,  1.0f,
								  1.0f, -1.0f, -1.0f,
								  1.0f, -1.0f,  1.0f,
								  
								 -1.0f,  1.0f, -1.0f,
								 -1.0f,  1.0f,  1.0f,
								  1.0f,  1.0f, -1.0f,
								  1.0f,  1.0f,  1.0f,
								  
								 -1.0f, -1.0f, -1.0f,
								 -1.0f, -1.0f,  1.0f,
								 -1.0f,  1.0f, -1.0f,
								 -1.0f,  1.0f,  1.0f,
								 
								  1.0f,  -1.0f, -1.0f,
								  1.0f,  -1.0f,  1.0f,
								  1.0f,   1.0f, -1.0f,
								  1.0f,   1.0f,  1.0f,
								  
								 -1.0f,  -1.0f, -1.0f,
								 -1.0f,   1.0f, -1.0f,
								  1.0f,  -1.0f, -1.0f,
								  1.0f,   1.0f, -1.0f,
								  
								 -1.0f,  -1.0f,  1.0f,
								 -1.0f,   1.0f,  1.0f,
								  1.0f,  -1.0f,  1.0f,
								  1.0f,   1.0f,  1.0f,
								 };

	float normals[6 * 4 * 3] = {-1.0f, -1.0f, -1.0f,
								 -1.0f, -1.0f,  1.0f,
								  1.0f, -1.0f, -1.0f,
								  1.0f, -1.0f,  1.0f,
								  
								 -1.0f,  1.0f, -1.0f,
								 -1.0f,  1.0f,  1.0f,
								  1.0f,  1.0f, -1.0f,
								  1.0f,  1.0f,  1.0f,
								  
								 -1.0f, -1.0f, -1.0f,
								 -1.0f, -1.0f,  1.0f,
								 -1.0f,  1.0f, -1.0f,
								 -1.0f,  1.0f,  1.0f,
								 
								  1.0f,  -1.0f, -1.0f,
								  1.0f,  -1.0f,  1.0f,
								  1.0f,   1.0f, -1.0f,
								  1.0f,   1.0f,  1.0f,
								  
								 -1.0f,  -1.0f, -1.0f,
								 -1.0f,   1.0f, -1.0f,
								  1.0f,  -1.0f, -1.0f,
								  1.0f,   1.0f, -1.0f,
								  
								 -1.0f,  -1.0f,  1.0f,
								 -1.0f,   1.0f,  1.0f,
								  1.0f,  -1.0f,  1.0f,
								  1.0f,   1.0f,  1.0f,
								 };
	
	float texcoords[6 * 4 * 2] = {0.0f, 0.0f,
								 0.0f, 1.0f,
								 1.0f, 0.0f,
								 1.0f, 1.0f,
								  
								 0.0f, 0.0f,
								 0.0f, 1.0f,
								 1.0f, 0.0f,
								 1.0f, 1.0f,
								  
								 0.0f, 0.0f,
								 0.0f, 1.0f,
								 1.0f, 0.0f,
								 1.0f, 1.0f,
								 
								 0.0f, 0.0f,
								 0.0f, 1.0f,
								 1.0f, 0.0f,
								 1.0f, 1.0f,
								  
								 0.0f, 0.0f,
								 0.0f, 1.0f,
								 1.0f, 0.0f,
								 1.0f, 1.0f,
								  
								 0.0f, 0.0f,
								 0.0f, 1.0f,
								 1.0f, 0.0f,
								 1.0f, 1.0f,
								 };
						 
	unsigned int indices[12 * 3] = {0, 2, 1, 
	                               1, 2, 3,
								   
								   4, 5, 6, 
	                               5, 7, 6,
								   
								   0+8, 1+8, 2+8, 
	                               1+8, 3+8, 2+8,
								   
								   4+8, 6+8, 5+8, 
	                               5+8, 6+8, 7+8,
								   
								   0+16, 1+16, 2+16, 
	                               1+16, 3+16, 2+16,
								   
								   4+16, 6+16, 5+16, 
	                               5+16, 6+16, 7+16,
								   };
	
	MeshData meshData;

	createMeshData(meshData, vertices, normals, texcoords, indices)

	return createMesh(&meshData);
}



VertexArray createRect(){
	float vertices[] = {-1.0f, 1.0f, -1.0f,
						-1.0f, -3.0f, -1.0f,
						 3.0f, 1.0f, -1.0f
						 };

	float normals[4 * 3] = {-1.0f, -1.0f, -1.0f,
								 -1.0f, -1.0f,  1.0f,
								  1.0f, -1.0f, -1.0f,
								  1.0f, -1.0f,  1.0f
								 };
	
	float texcoords[4 * 2] = {0.0f, 0.0f,
								 0.0f, 1.0f,
								 1.0f, 0.0f,
								 1.0f, 1.0f
								 };
						 
	unsigned int indices[2 * 3] = {0, 2, 1};
	
	MeshData meshData;

	createMeshData(meshData, vertices, normals, texcoords, indices)

	return createMesh(&meshData);
}




#endif










