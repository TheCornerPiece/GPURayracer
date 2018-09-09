#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include "linmath.h"

enum tags{
    STATIC,
    PLAYER,
    ENEMY,
    GOAL
};

struct Entity{
    int tag;
	vec4 pos;
	vec3 size;
	quat rot;
	VertexArray *mesh;
	GLuint *tex;
};

struct GameData{
    struct Entity entities[50];
    int entityCount;
};

void add_entity(struct GameData *gameData, int tag, float x, float y, float z, float w, float h, float l, float angle, VertexArray *vao){
    gameData->entities[gameData->entityCount].tag = tag;
	gameData->entities[gameData->entityCount].pos[0] = x;
	gameData->entities[gameData->entityCount].pos[1] = y;
	gameData->entities[gameData->entityCount].pos[2] = z;
	gameData->entities[gameData->entityCount].size[0] = w;
	gameData->entities[gameData->entityCount].size[1] = h;
	gameData->entities[gameData->entityCount].size[2] = l;
    quat rot;
    vec3 axis = {0.0f, 1.0f, 0.0f};
    quat_rotate(rot, angle, axis);
    gameData->entities[gameData->entityCount].rot[0] = rot[0];
    gameData->entities[gameData->entityCount].rot[1] = rot[1];
    gameData->entities[gameData->entityCount].rot[2] = rot[2];
    gameData->entities[gameData->entityCount].rot[3] = rot[3];
	gameData->entities[gameData->entityCount].mesh = vao;
	gameData->entityCount++;
}


void updateGoal(struct GameData *gameData, struct Entity *player, float dt){
    vec3 diff;
    struct Entity *entity;
    for (int ent = 0; ent < gameData->entityCount; ent++){
        entity = &gameData->entities[ent];
        if (entity->tag == GOAL){
            entity->rot[0] += dt;
            vec3_sub(diff, player->pos, entity->pos);
            if (vec3_len(diff) < 3.0f){
                printf("hit goal");
            }
        }
    }
}


void updateEnemies(struct GameData *gameData, struct Entity *player, float dt){
    vec3 diff;
    struct Entity *entity;
    for (int ent = 0; ent < gameData->entityCount; ent++) {
        entity = &gameData->entities[ent];
        if (entity->tag == ENEMY) {
            vec3_sub(diff, player->pos, entity->pos);
            vec3_scale(diff, diff, 1.0f * dt);
            vec3_add(entity->pos, entity->pos, diff);
            //printf("(%f, %f, %f)", entity->pos[0], entity->pos[1], entity->pos[2]);
        }
    }
}

void drawEntities(struct GameData *gameData, int modelLoc, mat4x4 model)
{
    struct Entity *entity;
    for (int ent = 0; ent < gameData->entityCount; ent++) {
        entity = &gameData->entities[ent];
        if (entity->mesh != NULL){
            glBindVertexArray(entity->mesh->vao);
            mat4x4_identity(model);
            mat4x4_translate_in_place(model, entity->pos[0], entity->pos[1], entity->pos[2]);
            mat4x4_rotate(model, model, entity->rot[0], entity->rot[1], entity->rot[2], entity->rot[3]);
            mat4x4_scale_aniso(model, model, entity->size[0], entity->size[1], entity->size[2]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat *)model);
            glDrawElements(GL_TRIANGLES, entity->mesh->numIndices, GL_UNSIGNED_INT, NULL);
            //glDrawArrays(GL_TRIANGLES, entity->mesh->numIndices, GL_UNSIGNED_INT, NULL);
        }
	}
}


#endif