#include "scene.h"
#include "texture.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <obj/transform.h>
#include <SDL2/SDL_opengl.h>

void init_scene(Scene* scene)
{
    scene->floor_texture = load_texture("assets/textures/floor.png");
    scene->help_texture = load_texture("assets/textures/help.png");
    scene->light_intensity = 0.5f; // Alapértelmezett fényerő

    load_model(&(scene->cat_model), "assets/models/Cat.obj");
    scene->cat_texture = load_texture("assets/textures/Cat.png");
    scale_model(&(scene->cat_model), 0.2, 0.2, 0.2);
}

void render_scene(const Scene* scene)
{
    float light_val = scene->light_intensity;
    GLfloat light_ambient[] = { light_val, light_val, light_val, 1.0f };
    GLfloat light_diffuse[] = { light_val, light_val, light_val, 1.0f };
    GLfloat light_pos[] = { 0.0f, 10.0f, 0.0f, 1.0f };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

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
    
    glPushMatrix();
        glTranslatef(-3.0f, 0.0f, 0.0f); // Elhelyezzük a padlón
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, scene->cat_texture);
        draw_model(&(scene->cat_model));
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void render_help(const Scene* scene) {
    glDisable(GL_LIGHTING); // A súgóhoz nem kell fény
    glDisable(GL_DEPTH_TEST); // Legyen minden előtt
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1); // 2D koordináta-rendszer 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, scene->help_texture);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(1, 0); glVertex2f(800, 0);
        glTexCoord2f(1, 1); glVertex2f(800, 600);
        glTexCoord2f(0, 1); glVertex2f(0, 600);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}