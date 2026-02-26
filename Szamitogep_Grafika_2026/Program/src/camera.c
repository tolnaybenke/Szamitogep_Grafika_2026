#include "camera.h"
#include <SDL2/SDL_opengl.h>
#include <math.h>

#define M_PI 3.14159265358979323846

void init_camera(Camera* camera)
{
    camera->position[0] = 0.0;
    camera->position[1] = 2.0; // Szemmagasság
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

    // Előre-hátra mozgás (X és Z tengelyen a yaw szög alapján)
    if (camera->is_moving_forward) {
        camera->position[0] += sin(angle) * speed;
        camera->position[2] -= cos(angle) * speed;
    }
    if (camera->is_moving_backward) {
        camera->position[0] -= sin(angle) * speed;
        camera->position[2] += cos(angle) * speed;
    }

    // Oldalazás (Strafing) - 90 fokkal eltolt szöggel
    if (camera->is_moving_left) {
        camera->position[0] -= cos(angle) * speed;
        camera->position[2] -= sin(angle) * speed;
    }
    if (camera->is_moving_right) {
        camera->position[0] += cos(angle) * speed;
        camera->position[2] += sin(angle) * speed;
    }
}

void set_view(const Camera* camera)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // A forgatások sorrendje és iránya
    glRotatef(camera->rotation[0], 1.0, 0, 0); // Pitch
    glRotatef(camera->rotation[1], 0, 1.0, 0); // Yaw
    
    // A világot mozgatjuk a kamerához képest negatív irányba
    glTranslatef(-camera->position[0], -camera->position[1], -camera->position[2]);
}