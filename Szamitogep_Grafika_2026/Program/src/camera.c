#include "camera.h"
#include <SDL2/SDL_opengl.h>
#include <math.h>

#define M_PI 3.14159265358979323846

void init_camera(Camera* camera)
{
    camera->position[0] = 0.0;
    camera->position[1] = 2.0;
    camera->position[2] = 5.0;

    camera->rotation[0] = 0.0;
    camera->rotation[1] = 0.0;
    camera->rotation[2] = 0.0;

    camera->is_moving_forward = false;
    camera->is_moving_backward = false;
    camera->is_moving_left = false;
    camera->is_moving_right = false;
}

void update_camera(Camera* camera, double time)
{
    double angle = camera->rotation[1] * M_PI / 180.0;
    double speed = 5.0 * time;

    double room_limit = 9.8;

    double new_x = camera->position[0];
    double new_z = camera->position[2];

    if (camera->is_moving_forward) {
        new_x += sin(angle) * speed;
        new_z -= cos(angle) * speed;
    }
    if (camera->is_moving_backward) {
        new_x -= sin(angle) * speed;
        new_z += cos(angle) * speed;
    }

    if (camera->is_moving_left) {
        new_x -= cos(angle) * speed;
        new_z -= sin(angle) * speed;
    }
    if (camera->is_moving_right) {
        new_x += cos(angle) * speed;
        new_z += sin(angle) * speed;
    }

    // --- ÜTKÖZÉSVIZSGÁLAT (COLLISION DETECTION) ---
    if (new_x > -room_limit && new_x < room_limit) {
        camera->position[0] = new_x;
    }

    if (new_z > -room_limit && new_z < room_limit) {
        camera->position[2] = new_z;
    }
}

void set_view(const Camera* camera)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(camera->rotation[0], 1.0, 0, 0);
    glRotatef(camera->rotation[1], 0, 1.0, 0);
    
    glTranslatef(-camera->position[0], -camera->position[1], -camera->position[2]);
}