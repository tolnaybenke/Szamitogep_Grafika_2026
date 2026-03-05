#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include <obj/model.h>
#include <SDL2/SDL_opengl.h>

typedef struct Scene
{
    GLuint floor_texture;
    GLuint help_texture;
    float light_intensity;

    Model cat_model;
    GLuint cat_texture;
} Scene;

void init_scene(Scene* scene);
void render_scene(const Scene* scene);
void render_help(const Scene* scene);

#endif /* SCENE_H */