#include <stdlib.h>
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
    
    load_model(&(scene->plant_b_model), "assets/models/plant_b.obj");
    scene->plant_b_texture = load_texture("assets/textures/plant_b.png");
    scale_model(&(scene->plant_b_model), 1, 1, 1);
    
    // Textúrák betöltése
    scene->wall_texture = load_texture("assets/textures/mossy_brick_wall.png");
    scene->roof_texture = load_texture("assets/textures/roof.png");
    scene->floor_texture = load_texture("assets/textures/floor.png");

    // 1. Északi fal (0, -9)
    scene->pots[0].x = 0.0f;  scene->pots[0].z = -9.0f;
    // 2. Déli fal (0, 9)
    scene->pots[1].x = 0.0f;  scene->pots[1].z = 9.0f;
    // 3. Keleti fal (9, 0)
    scene->pots[2].x = 9.0f;  scene->pots[2].z = 0.0f;
    // 4. Nyugati fal (-9, 0)
    scene->pots[3].x = -9.0f; scene->pots[3].z = 0.0f;

    for (int i = 0; i < 4; i++) {
        scene->pots[i].active_plant = 0;
        scene->pots[i].is_selected = false;
    }

    scene->watering_timer = 0.0;

    for (int i = 0; i < MAX_WATER_DROPS; i++) {
        // Véletlenszerű helyek a plafon alatt (y=3.8 környékén)
        scene->drops[i].x = (float)(rand() % 2000) / 100.0f - 10.0f; // -10 és 10 között
        scene->drops[i].z = (float)(rand() % 2000) / 100.0f - 10.0f;
        scene->drops[i].y = 4.0f + (float)(rand() % 400) / 100.0f;   // Kicsit a plafon felett indulnak
        scene->drops[i].speed = 2.0f + (float)(rand() % 300) / 100.0f; // Eltérő esési sebesség
    }
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
        glBindTexture(GL_TEXTURE_2D, scene->potted_plant_1_texture);
        draw_model(&(scene->potted_plant_1_model));
        glDisable(GL_ALPHA_TEST);
    glPopMatrix();
    /*
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
    */

    for (int i = 0; i < 4; i++) {
        glPushMatrix();
            // 1. ELTOLÁS A HELYÉRE (A váza nem forog, így a hitbox fix marad!)
            glTranslatef(scene->pots[i].x, 0.0f, scene->pots[i].z);

            // --- KIJELÖLÉS JELZÉSE ---
            if (scene->pots[i].is_selected) {
                // Erős piros fény, ha ki van jelölve
                GLfloat emission[] = { 0.7f, 0.0f, 0.0f, 1.0f };
                glMaterialfv(GL_FRONT, GL_EMISSION, emission);
            } else {
                GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
                glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
            }

            // --- CSERÉP RAJZOLÁSA ---
            glBindTexture(GL_TEXTURE_2D, scene->pot_texture);
            draw_model(&(scene->pot_model));
            
            // Fényvisszaadás alaphelyzetbe a növény előtt
            GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);

            // --- NÖVÉNY RAJZOLÁSA (HA VAN BENNE) ---
            if (scene->pots[i].active_plant > 0) {
                glPushMatrix();
                    // Emeljük meg a növényt, hogy a cserépben legyen
                    glTranslatef(0.0f, 1.0f, 0.0f);

                    // --- FORGATÁS AZ INDEX (i) ALAPJÁN ---
                    // i=0: Észak (eredeti), i=1: Dél (180 fok), i=2: Kelet (90 fok), i=3: Nyugat (-90 fok)
                    if (i == 1) glRotatef(180.0f, 0, 1, 0);
                    else if (i == 2) glRotatef(90.0f, 0, 1, 0);
                    else if (i == 3) glRotatef(-90.0f, 0, 1, 0);
                    
                    glEnable(GL_ALPHA_TEST);
                    glAlphaFunc(GL_GREATER, 0.1f);

                    if (scene->pots[i].active_plant == 1) {
                        glBindTexture(GL_TEXTURE_2D, scene->plant_a_texture);
                        draw_model(&(scene->plant_a_model));
                    } else if (scene->pots[i].active_plant == 2) {
                        glBindTexture(GL_TEXTURE_2D, scene->plant_b_texture);
                        draw_model(&(scene->plant_b_model));
                    }

                    glDisable(GL_ALPHA_TEST);
                glPopMatrix();
            }
        glPopMatrix();
    }

    if (scene->watering_timer > 0.0) {
        glDisable(GL_TEXTURE_2D); // A víznek nem kell textúra
        glDisable(GL_LIGHTING);   // A víz legyen fényes magától is

        glBegin(GL_LINES);
        for (int i = 0; i < MAX_WATER_DROPS; i++) {
            // A víz színe legyen világoskék, némi átlátszósággal
            glColor4f(0.5f, 0.5f, 1.0f, 0.8f);
            
            // Egy rövid függőleges vonal (a csepp)
            glVertex3f(scene->drops[i].x, scene->drops[i].y, scene->drops[i].z);
            glVertex3f(scene->drops[i].x, scene->drops[i].y - 0.1f, scene->drops[i].z);
        }
        glEnd();

        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }

    glDisable(GL_TEXTURE_2D);
}

void update_scene(Scene* scene, double dt) {
    // Ha fut az öntözés (az időzítő pozitív)
    if (scene->watering_timer > 0.0) {
        scene->watering_timer -= dt;

        for (int i = 0; i < MAX_WATER_DROPS; i++) {
            scene->drops[i].y -= scene->drops[i].speed * dt;

            // Ha leért a földre (y < 0), visszaugrik a tetejére
            if (scene->drops[i].y < 0.0f) {
                scene->drops[i].y = 4.0f;
            }
        }
    }
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