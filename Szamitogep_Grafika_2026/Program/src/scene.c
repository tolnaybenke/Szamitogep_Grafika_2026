#include "scene.h"
#include "texture.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <obj/transform.h>
#include <SDL2/SDL_opengl.h>

void init_scene(Scene* scene)
{
    scene->help_texture = load_texture("assets/textures/help.png");
    scene->light_intensity = 0.5f; // Alapértelmezett fényerő

    load_model(&(scene->potted_plant_1_model), "assets/models/potted_plant_1.obj");
    scene->potted_plant_1_texture = load_texture("assets/textures/potted_plant_1.png");
    scale_model(&(scene->potted_plant_1_model), 4, 4, 4);

    load_model(&(scene->pot_model), "assets/models/pot.obj");
    scene->pot_texture = load_texture("assets/textures/pot.png");
    scale_model(&(scene->pot_model), 3, 3, 3);
    
    load_model(&(scene->plant_a_model), "assets/models/plant_a.obj");
    scene->plant_a_texture = load_texture("assets/textures/plant_a.png");
    scale_model(&(scene->plant_a_model), 1, 1, 1);
    /*
    load_model(&(scene->plant_b_model), "assets/models/plant_b.obj");
    scene->plant_b_texture = load_texture("assets/textures/plant_b.png");
    scale_model(&(scene->plant_b_model), 4, 4, 4);
    */
    // Textúrák betöltése
    scene->wall_texture = load_texture("assets/textures/mossy_brick_wall.png");
    scene->roof_texture = load_texture("assets/textures/roof.png");
    scene->floor_texture = load_texture("assets/textures/floor.png");
}

void render_scene(const Scene* scene)
{
    float val = scene->light_intensity;

    // Fény összetevői
    GLfloat ambient[] = { val * 0.4f, val * 0.4f, val * 0.4f, 1.0f };
    GLfloat diffuse[] = { val, val, val, 1.0f };
    GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat light_pos[] = { 0.0f, 3.8f, 0.0f, 1.0f };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Konstans, lineáris és kvadratikus tompítás beállítása
    /*glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);*/

    // Anyagjellemzők csillogásának kikapcsolása
    GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //

    float size = 10.0f;
    float height = 4.0f;

    // Falak TODO: négyszögekre darabolás
    glBindTexture(GL_TEXTURE_2D, scene->wall_texture);
    glBegin(GL_QUADS);
        // Hátsó
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0, 0); glVertex3f(-size, 0, -size);
        glTexCoord2f(5, 0); glVertex3f( size, 0, -size);
        glTexCoord2f(5, 2); glVertex3f( size, height, -size);
        glTexCoord2f(0, 2); glVertex3f(-size, height, -size);

        // Első
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0, 0); glVertex3f( size, 0, size);
        glTexCoord2f(5, 0); glVertex3f(-size, 0, size);
        glTexCoord2f(5, 2); glVertex3f(-size, height, size);
        glTexCoord2f(0, 2); glVertex3f( size, height, size);

        // Bal
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(-size, 0,  size);
        glTexCoord2f(5, 0); glVertex3f(-size, 0, -size);
        glTexCoord2f(5, 2); glVertex3f(-size, height, -size);
        glTexCoord2f(0, 2); glVertex3f(-size, height,  size);

        // Jobb
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(size, 0, -size);
        glTexCoord2f(5, 0); glVertex3f(size, 0,  size);
        glTexCoord2f(5, 2); glVertex3f(size, height,  size);
        glTexCoord2f(0, 2); glVertex3f(size, height, -size);
    glEnd();

    // Plafon
    glBindTexture(GL_TEXTURE_2D, scene->roof_texture);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(-size, height, -size);
        glTexCoord2f(5, 0); glVertex3f( size, height, -size);
        glTexCoord2f(5, 5); glVertex3f( size, height,  size);
        glTexCoord2f(0, 5); glVertex3f(-size, height,  size);
    glEnd();

    // Padló
    glBindTexture(GL_TEXTURE_2D, scene->floor_texture);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(-size, 0, -size);
        glTexCoord2f(5, 0); glVertex3f( size, 0, -size);
        glTexCoord2f(5, 5); glVertex3f( size, 0,  size); 
        glTexCoord2f(0, 5); glVertex3f(-size, 0,  size);
    glEnd();

    glPushMatrix();
        glTranslatef(-7.0f, 0.0f, 7.0f); // Elhelyezzük a padlón
        glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);
        //glDisable(GL_CULL_FACE); // Hogy a levelek hátulja is látszódjon
        glBindTexture(GL_TEXTURE_2D, scene->potted_plant_1_texture);
        draw_model(&(scene->potted_plant_1_model));
        //glEnable(GL_CULL_FACE);
        glDisable(GL_ALPHA_TEST);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, scene->pot_texture);
        draw_model(&(scene->pot_model));
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 1.0f, 0.0f);
        glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, scene->plant_a_texture);
        draw_model(&(scene->plant_a_model));
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