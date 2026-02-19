#include "camera.h"
#include <SDL2/SDL_opengl.h>
#include <math.h>

#define M_PI 3.14159265358979323846

void init_camera(Camera* camera)
{
    camera->position[0] = 0.0;
    camera->position[1] = 2.0; // Szemmagasság a padló felett
    camera->position[2] = 5.0;

    camera->rotation[0] = 0.0;
    camera->rotation[1] = 0.0;
    camera->rotation[2] = 0.0;

    camera->speed[0] = 0.0;
    camera->speed[1] = 0.0;
    camera->speed[2] = 0.0;

    camera->is_moving_forward = false;
    camera->is_moving_backward = false;
    camera->is_moving_left = false;
    camera->is_moving_right = false;
}

void update_camera(Camera* camera, double time)
{
    double angle;
    double side_angle;
    double speed = 4.0; // Mozgás sebessége

    // Sebesség számítása a gombok alapján
    camera->speed[2] = 0; // Előre/hátra (Z)
    camera->speed[0] = 0; // Oldalra (X)

    if (camera->is_moving_forward) camera->speed[2] = speed;
    if (camera->is_moving_backward) camera->speed[2] = -speed;
    if (camera->is_moving_left) camera->speed[0] = -speed;
    if (camera->is_moving_right) camera->speed[0] = speed;

    // Mozgásirány kiszámítása a kamera elforgatása (Y tengely körüli forgás, azaz yaw) alapján
    angle = camera->rotation[1] * M_PI / 180.0;

    // Csak X és Z tengelyen módosítjuk a pozíciót! (Y változatlan marad)
    if (camera->speed[2] != 0.0 || camera->speed[0] != 0.0) {
        camera->position[0] += cos(angle) * camera->speed[0] * time + sin(angle) * camera->speed[2] * time;
        camera->position[2] += sin(angle) * camera->speed[0] * time - cos(angle) * camera->speed[2] * time;
    }
}

void set_view(const Camera* camera)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Kamera forgatása (X tengely = fel/le nézés, Y tengely = balra/jobbra nézés)
    glRotatef(-(camera->rotation[0]), 1.0, 0, 0);
    glRotatef(-(camera->rotation[1]), 0, 1.0, 0);

    // Kamera pozícionálása
    glTranslatef(-(camera->position[0]), -(camera->position[1]), -(camera->position[2]));
}