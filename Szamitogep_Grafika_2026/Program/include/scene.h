#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include <SDL2/SDL_opengl.h>

typedef struct Scene
{
    GLuint floor_texture;
} Scene;

void init_scene(Scene* scene);
void render_scene(const Scene* scene);

#endif /* SCENE_H */