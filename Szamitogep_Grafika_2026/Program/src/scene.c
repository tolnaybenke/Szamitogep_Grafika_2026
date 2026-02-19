#include "scene.h"
#include "texture.h"
#include <SDL2/SDL_opengl.h>

void init_scene(Scene* scene)
{
    // Cseréld le a fájlnevet a saját textúrád útvonalára!
    scene->floor_texture = load_texture("assets/textures/floor.png");
}

void render_scene(const Scene* scene)
{
    // Környezeti fény bekapcsolása a padlóhoz
    GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    glEnable(GL_LIGHTING);

    // Padló kirajzolása (Egy hatalmas, textúrázott négyzet)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, scene->floor_texture);

    glBegin(GL_QUADS);
        // Ismétlődő textúra a padlón (a textúra koordinátákat nagyobbra vesszük)
        glTexCoord2f(0.0f, 0.0f);   glVertex3f(-50.0f, 0.0f, -50.0f);
        glTexCoord2f(50.0f, 0.0f);  glVertex3f( 50.0f, 0.0f, -50.0f);
        glTexCoord2f(50.0f, 50.0f); glVertex3f( 50.0f, 0.0f,  50.0f);
        glTexCoord2f(0.0f, 50.0f);  glVertex3f(-50.0f, 0.0f,  50.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}