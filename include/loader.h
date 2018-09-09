#ifndef LOADER_H
#define LOADER_H

#include <stdio.h>
#include "mesh.h"

#ifndef _WIN32

#pragma pack(push, 1)

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t LONG;

typedef struct tagBITMAPFILEHEADER 
{ WORD bfType; 
  DWORD bfSize; 
  WORD bfReserved1; 
  WORD bfReserved2; 
  DWORD bfOffBits; 
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

#pragma pack(pop)

#endif

struct FileInfo{
	char* buffer;
	long length;
};


uint16_t bswap16(uint16_t x) { return ((x & 0xFF) << 8) | ((x & 0xFF00) >> 8); }


struct FileInfo loadFile(const char *name, const int addEsc)
{
	FILE *fp = fopen(name, "rb");
	struct FileInfo info;
	info.length = 0;
	
	if (fp) {
		fseek(fp, 0, SEEK_END);
		info.length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		info.buffer = (char*) malloc((info.length + addEsc) * sizeof(char));
		
		if (info.buffer)
		{
			fread(info.buffer, sizeof(char), info.length, fp);
			if (addEsc)
			{
				info.buffer[info.length] = '\0';
			}
		}
		fclose(fp);
	}
	
	return info;
}

unsigned char *loadBinary(const char *name, size_t *size)
{
	FILE *fp = fopen(name, "rb");
	unsigned char *buffer = NULL;
	
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		(*size) = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buffer = (char*) malloc((*size) * sizeof(char));
		
		if (buffer)
		{
			fread(buffer, sizeof(char), *size, fp);
		}
		fclose(fp);
	}
	
	return buffer;
}

unsigned char* decodeBMP(const unsigned char *mem, size_t size, int *width, int *height)
{
	unsigned char *buffer = NULL;
	
	size_t offset = 0;
	BITMAPFILEHEADER header = *(PBITMAPFILEHEADER) mem;

	offset += sizeof(BITMAPFILEHEADER);
	switch(bswap16(header.bfType)){
		case 'B'*256+'M': // BM
		{
			BITMAPINFOHEADER info = *(PBITMAPINFOHEADER) &mem[offset];
			offset = header.bfOffBits;
			*width = info.biWidth;
			*height = info.biHeight;
			
			size_t pixelSize = info.biBitCount / 8;
			
			printf("%d\n", pixelSize);
			printf("%d\n", sizeof(char));
			
			size_t bufferSize = pixelSize * (*width) * (*height) * sizeof(unsigned char);
			
			buffer = (unsigned char *) malloc(bufferSize);
			
			memcpy(buffer, &mem[offset], bufferSize);
			
			break;
		}
	}
	
	return buffer;
}


GLuint loadTexture(char *filename){
	unsigned char *bmpData;
	size_t bmpSize;
	int bmpWidth;
	int bmpHeight;
	bmpData = loadBinary(filename, &bmpSize);
	
	unsigned char *pixelData;
	pixelData = decodeBMP(bmpData, bmpSize, &bmpWidth, &bmpHeight);
	
	GLuint texId; 
	glGenTextures(1, &texId);
	
	glBindTexture(GL_TEXTURE_2D, texId);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
				 bmpWidth, bmpHeight, 0,
				 GL_BGR, GL_UNSIGNED_BYTE, pixelData);
				 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	free(bmpData);
	free(pixelData);
}


MeshData objLoader(const char *filename){
	FILE *fp = fopen(filename, "rb");
	char lineBuffer[256];

	char *pending;
	float x;
	float y;
	float z;
	int vertIndex;
	int texIndex;
	int normIndex;
	int place;

	int maxVerts = 256;

	float vertices[maxVerts*3];
	float normals[maxVerts*3];
	float texcoords[maxVerts*2];
	int faces[maxVerts*9];

	int currentVertex = 0;
	int currentNormal = 0;
	int currentTexCoord = 0;
	int currentFace = 0;

	while (fgets(lineBuffer, 256, fp) != NULL){
		switch (lineBuffer[0]){
			case '#':
				break;

			case 'o':
				//printf("object: %s", &lineBuffer[2]);
				break;

			case 'v':
				switch(lineBuffer[1]){
					case ' ':
						x = strtof(&lineBuffer[2], &pending);
						y = strtof(pending, &pending);
						z = strtof(pending, NULL);
						//printf("vertex: (%f, %f, %f)\n", x, y, z);
						vertices[currentVertex] = x;
						vertices[currentVertex + 1] = y;
						vertices[currentVertex + 2] = z;
						currentVertex += 3;
						break;

					case 't':
						x = strtof(&lineBuffer[3], &pending);
						y = strtof(pending, NULL);
						texcoords[currentTexCoord] = x;
						texcoords[currentTexCoord + 1] = y;
						currentTexCoord += 2;

						//printf("texcoord: (%f, %f)\n", x, y);
						break;
					
					case 'n':
						x = strtof(&lineBuffer[2], &pending);
						y = strtof(pending, &pending);
						z = strtof(pending, NULL);
						normals[currentNormal] = x;
						normals[currentNormal + 1] = y;
						normals[currentNormal + 2] = z;
						currentNormal += 3;
						
						//printf("normal: (%f, %f, %f)\n", x, y, z);
						break;
					
					//default:
						//printf("invalid vertex: %s", lineBuffer);
				}
				break;
			
			case 'f':
				//printf(lineBuffer);
				place = 0;
				while (1){
					if (sscanf(&lineBuffer[2+place], "%d", &vertIndex) == 1)
					{
						//printf("%d", vertIndex);
						faces[currentFace] = vertIndex;
						currentFace++;
						if (vertIndex < 10){
							place += 2;
						}
						else if (vertIndex < 100){
							place += 3;
						}
						else if (vertIndex < 1000){
							place += 4;
						}
						
					} else {
						break;
					}
				}
				//printf("current face: %d\n", currentFace);
				break;
			
			//default:
				//printf("\ninvalid character: %s", lineBuffer);
		}
	}

	fclose(fp);

	GLfloat finalVertices[currentFace];
	GLfloat finalNormals[currentFace];
	GLfloat finalTexcoords[currentFace / 3 * 2];
	unsigned int indices[currentFace / 3];

	//printf("\n%d / 3 = %d\n", currentFace, currentFace / 3);

	int index;

	//printf("\nface len: %d", currentFace);
	//printf("\nmax indices: %d", currentFace / 3);

	int vert_start, tc_start, norm_start;

	for (int pointIndex = 0; pointIndex < currentFace / 3; pointIndex++){
		indices[pointIndex] = pointIndex;

		vert_start = (faces[pointIndex * 3] - 1) * 3;
		tc_start = (faces[pointIndex * 3 + 1] - 1) * 2;
		norm_start = (faces[pointIndex * 3 + 2] - 1) * 3;

		//printf("\nvs: %d, ts: %d, ns: %d", vert_start, tc_start, norm_start);

		finalVertices[pointIndex * 3] = vertices[vert_start];
		finalVertices[pointIndex * 3 + 1] = vertices[vert_start + 1];
		finalVertices[pointIndex * 3 + 2] = vertices[vert_start + 2];

		finalTexcoords[pointIndex * 2] = texcoords[tc_start];
		finalTexcoords[pointIndex * 2 + 1] = texcoords[tc_start + 1];

		finalNormals[pointIndex * 3] = normals[norm_start];
		finalNormals[pointIndex * 3 + 1] = normals[norm_start + 1];
		finalNormals[pointIndex * 3 + 2] = normals[norm_start + 2];

		#if 0
		printf("\n%d(%d) - vertex: (%f, %f, %f), tc: (%f, %f), normal: (%f, %f, %f)", 
			pointIndex,
			indices[pointIndex],
			finalVertices[pointIndex * 3],
			finalVertices[pointIndex * 3 + 1],
			finalVertices[pointIndex * 3 + 2],
			finalTexcoords[pointIndex * 2],
			finalTexcoords[pointIndex * 2 + 1],
			finalNormals[pointIndex * 3],
			finalNormals[pointIndex * 3 + 1],
			finalNormals[pointIndex * 3 + 2]
			);
		#endif
	}

	int pointIndex=5;
	
	#if 0
	printf("\n%d(%d) - vertex: (%f, %f, %f), tc: (%f, %f), normal: (%f, %f, %f)", 
			   pointIndex,
			   indices[pointIndex],
			   finalVertices[pointIndex * 3],
			   finalVertices[pointIndex * 3 + 1],
			   finalVertices[pointIndex * 3 + 2],
			   finalTexcoords[pointIndex * 2],
			   finalTexcoords[pointIndex * 2 + 1],
			   finalNormals[pointIndex * 3 ],
			   finalNormals[pointIndex * 3 + 1],
			   finalNormals[pointIndex * 3 + 2]
			   );
	#endif

	MeshData meshData;
	
	createMeshData(meshData, finalVertices, finalNormals, finalTexcoords, indices)
	
	return meshData;
}


#endif