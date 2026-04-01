#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include <obj/model.h>
#include <SDL2/SDL_opengl.h>

typedef struct Scene
{
    GLuint floor_texture;
    GLuint help_texture;
    GLuint wall_texture;
    GLuint roof_texture;

    float light_intensity;

    Model potted_plant_1_model;
    Model pot_model;
    Model plant_a_model;
    Model plant_b_model;
    
    GLuint potted_plant_1_texture;
    GLuint pot_texture;
    GLuint plant_a_texture;
    GLuint plant_b_texture;

    int active_plant; // 0: üres, 1: "A" növény, 2: "B" növény
    bool is_pot_selected;
} Scene;

void init_scene(Scene* scene);
void render_scene(const Scene* scene);
void render_help(const Scene* scene);

#endif /* SCENE_H */