#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

typedef struct Camera
{
    double position[3];
    double rotation[3];
    double speed[3]; // Sebesség (előre/hátra, oldalra, fel/le - bár a fel/le most nem kell)
    bool is_moving_forward;
    bool is_moving_backward;
    bool is_moving_left;
    bool is_moving_right;
} Camera;

void init_camera(Camera* camera);
void update_camera(Camera* camera, double time);
void set_view(const Camera* camera);

#endif /* CAMERA_H */