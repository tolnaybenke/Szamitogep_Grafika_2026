#include <stdlib.h>
#include "scene.h"
#include "texture.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <obj/transform.h>
#include <SDL2/SDL_opengl.h>
#include <GL/freeglut.h> //HUD-szöveghez

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

    scene->selected_pot_index = -1;
    
    for (int i = 0; i < 4; i++) {
        scene->pots[i].active_plant = 0;
        scene->pots[i].is_selected = false;
    }

    scene->watering_timer = 0.0;
    scene->fog_density = 0.0f;

    for (int i = 0; i < MAX_WATER_DROPS; i++) {
        // Megnézzük, melyik cseréphez tartozik a csepp (0, 1, 2 vagy 3)
        int pot_index = i % 4; 

        // A cserép X és Z koordinátája + egy pici véletlen szórás (-0.5 és 0.5 között)
        float offset_x = ((float)(rand() % 100) / 100.0f) - 0.5f;
        float offset_z = ((float)(rand() % 100) / 100.0f) - 0.5f;

        scene->drops[i].x = scene->pots[pot_index].x + offset_x;
        scene->drops[i].z = scene->pots[pot_index].z + offset_z;
        
        // Magasság: a plafon (4.0) felett különböző pontokon kezdjenek
        scene->drops[i].y = 4.0f + (float)(rand() % 400) / 100.0f;
        scene->drops[i].speed = 3.0f + (float)(rand() % 400) / 100.0f;
    }
}

void render_scene(const Scene* scene)
{
    // --- KÖD BEÁLLÍTÁSA ---
    if (scene->fog_density > 0.0f) {
        glEnable(GL_FOG);
        //glFogi(GL_FOG_MODE, GL_EXP2); // Realisztikusabb, négyzetes sűrűség kicsit furcsán néz ki
        
        // Köd színe: világosszürke/kissé kékes (mint a pára)
        GLfloat fog_color[] = { 0.7f, 0.7f, 0.8f, 1.0f };
        glFogfv(GL_FOG_COLOR, fog_color);
        
        glFogf(GL_FOG_DENSITY, scene->fog_density);
        glHint(GL_FOG_HINT, GL_NICEST); // Legszebb minőség
    } else {
        glDisable(GL_FOG);
    }

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
    float step = 0.5f; // A négyszögek mérete: minél kisebb, annál szebb a fény (és lassabb a program). 0.5f egy jó középút.

    // 1. Falak: Körbefordulunk a szobában 90 fokos lépésekben
    glBindTexture(GL_TEXTURE_2D, scene->wall_texture);
    for (int wall = 0; wall < 4; wall++) {
        glPushMatrix();
            glRotatef(wall * 90.0f, 0.0f, 1.0f, 0.0f); // Beállítjuk az aktuális fal irányát
            glNormal3f(0.0f, 0.0f, 1.0f); // A normálvektor mindig a fal felületére merőleges (a tengely forgatása miatt működik így)
            
            glBegin(GL_QUADS);
            for (float x = -size; x < size; x += step) {
                for (float y = 0; y < height; y += step) {
                    
                    // Textúra koordináták arányosítása a falfelületen
                    // Az ismétlési faktorokat (pl. 5, 2) a ciklusokon belül tartjuk folyamatosnak
                    float tx1 = ((x + size) / (2 * size)) * 5.0f; 
                    float tx2 = ((x + step + size) / (2 * size)) * 5.0f;
                    float ty1 = (y / height) * 2.0f;
                    float ty2 = ((y + step) / height) * 2.0f;

                    glTexCoord2f(tx1, ty1); glVertex3f(x, y, -size);
                    glTexCoord2f(tx2, ty1); glVertex3f(x + step, y, -size);
                    glTexCoord2f(tx2, ty2); glVertex3f(x + step, y + step, -size);
                    glTexCoord2f(tx1, ty2); glVertex3f(x, y + step, -size);
                }
            }
            glEnd();
        glPopMatrix();
    }

    // 2. Plafon (Y=4.0): X és Z tengelyeken darabolunk
    glBindTexture(GL_TEXTURE_2D, scene->roof_texture);
    glNormal3f(0.0f, -1.0f, 0.0f); // Lefelé mutat
    glBegin(GL_QUADS);
    for (float x = -size; x < size; x += step) {
        for (float z = -size; z < size; z += step) {
            
            float tx1 = ((x + size) / (2 * size)) * 5.0f;
            float tx2 = ((x + step + size) / (2 * size)) * 5.0f;
            float ty1 = ((z + size) / (2 * size)) * 5.0f; // Itt ty1 a Z-tengelyt jelöli
            float ty2 = ((z + step + size) / (2 * size)) * 5.0f;

            glTexCoord2f(tx1, ty1); glVertex3f(x, height, z);
            glTexCoord2f(tx2, ty1); glVertex3f(x + step, height, z);
            glTexCoord2f(tx2, ty2); glVertex3f(x + step, height, z + step);
            glTexCoord2f(tx1, ty2); glVertex3f(x, height, z + step);
        }
    }
    glEnd();

    // 3. Padló (Y=0.0): X és Z tengelyeken darabolunk
    glBindTexture(GL_TEXTURE_2D, scene->floor_texture);
    glNormal3f(0.0f, 1.0f, 0.0f); // Felfelé mutat
    glBegin(GL_QUADS);
    for (float x = -size; x < size; x += step) {
        for (float z = -size; z < size; z += step) {
            
            float tx1 = ((x + size) / (2 * size)) * 5.0f;
            float tx2 = ((x + step + size) / (2 * size)) * 5.0f;
            float ty1 = ((z + size) / (2 * size)) * 5.0f;
            float ty2 = ((z + step + size) / (2 * size)) * 5.0f;

            glTexCoord2f(tx1, ty1); glVertex3f(x, 0.0f, z);
            glTexCoord2f(tx2, ty1); glVertex3f(x + step, 0.0f, z);
            glTexCoord2f(tx2, ty2); glVertex3f(x + step, 0.0f, z + step);
            glTexCoord2f(tx1, ty2); glVertex3f(x, 0.0f, z + step);
        }
    }
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
            // Váltakozó kék árnyalatok
            if (i % 2 == 0) glColor3f(0.4f, 0.4f, 1.0f);
            else glColor3f(0.6f, 0.6f, 1.0f);
            
            glVertex3f(scene->drops[i].x, scene->drops[i].y, scene->drops[i].z);
            glVertex3f(scene->drops[i].x, scene->drops[i].y - 0.15f, scene->drops[i].z);
        }
        glEnd();

        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }

    draw_hud(scene);

    glDisable(GL_TEXTURE_2D);
}

void update_scene(Scene* scene, double dt) {
    if (scene->watering_timer > 0.0) {
        scene->watering_timer -= dt;

        // Ha megy az öntözés, fokozatosan növeljük a sűrűséget 0.15-ig
        if (scene->fog_density < 0.08f) {
            scene->fog_density += (float)dt * 0.01f; // Sebesség, amivel ködösödik
        }

        for (int i = 0; i < MAX_WATER_DROPS; i++) {
            scene->drops[i].y -= scene->drops[i].speed * (float)dt;

            // Ha leért a földre
            if (scene->drops[i].y < 0.0f) {
                int pot_index = i % 4; // Ugyanaz a logika, mint az initnél
                
                // Újra a plafonra tesszük
                scene->drops[i].y = 4.0f;
                
                // Opcionális: minden körben kicsit változtathatjuk a szórást a cserép felett
                float offset_x = ((float)(rand() % 100) / 100.0f) - 0.5f;
                float offset_z = ((float)(rand() % 100) / 100.0f) - 0.5f;
                scene->drops[i].x = scene->pots[pot_index].x + offset_x;
                scene->drops[i].z = scene->pots[pot_index].z + offset_z;
            }
        }
    } else {
        // Ha nem megy az öntözés, a köd lassan elszáll 0-ig
        if (scene->fog_density > 0.0f) {
            scene->fog_density -= (float)dt * 0.02f; // Lassabban oszlik el, mint ahogy jön
        }
        if (scene->fog_density < 0.0f) scene->fog_density = 0.0f;
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

void draw_hud(const Scene* scene) {
    // 1. Átváltunk 2D módba
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 600, 0); // Fix 800x600-as HUD koordináták

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 2. Kikapcsoljuk a 3D-s dolgokat, hogy a szöveg tiszta legyen
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // 3. Szöveg összeállítása
    if (scene->selected_pot_index != -1) {
        int p_type = scene->pots[scene->selected_pot_index].active_plant;
        char* plant_name;
        
        if (p_type == 1) plant_name = "Kaktusz";
        else if (p_type == 2) plant_name = "Gyom";
        else plant_name = "Ures";

        char status_text[50];
        sprintf(status_text, "Kijelolt pot: %d | Tartalom: %s", scene->selected_pot_index + 1, plant_name);

        // Szöveg színe (legyen sárga vagy fehér, hogy látszódjon a ködben is)
        glColor3f(1.0f, 1.0f, 0.0f);
        
        // Szöveg pozíciója (bal felső sarok)
        glRasterPos2i(20, 30);

        // Kiírás karakterenként
        for (char* c = status_text; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    // 4. Mátrixok és állapotok visszaállítása
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}