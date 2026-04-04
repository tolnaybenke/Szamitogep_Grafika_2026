#include "app.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void perform_raycast(App* app, int mouse_x, int mouse_y) {
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLdouble posX1, posY1, posZ1, posX2, posY2, posZ2;

    // Lekérjük a képernyő méreteit
    glGetIntegerv(GL_VIEWPORT, viewport);

    // --- Mátrixok ideiglenes beállítása a visszavetítéshez ---
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(60.0, (double)viewport[2] / (double)viewport[3], 0.1, 100.0); // FIGYELEM: Ennek egyeznie kell a camera.c-ben lévő beállításoddal!
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    set_view(&(app->camera)); // Beállítjuk a kamerát, mintha rajzolnánk
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glPopMatrix();
    // --------------------------------------------------------

    // Egér Y koordinátájának megfordítása (mert a képernyőn Y lefelé nő, OpenGL-ben felfelé)
    float winY = (float)viewport[3] - (float)mouse_y;

    // Sugár kezdőpontja (Közeli vágósík)
    gluUnProject(mouse_x, winY, 0.0, modelview, projection, viewport, &posX1, &posY1, &posZ1);
    // Sugár végpontja (Távoli vágósík)
    gluUnProject(mouse_x, winY, 1.0, modelview, projection, viewport, &posX2, &posY2, &posZ2);

    // Sugár irányvektora
    double dirX = posX2 - posX1;
    double dirY = posY2 - posY1;
    double dirZ = posZ2 - posZ1;

    // Irányvektor normalizálása (hossz = 1)
    double length = sqrt(dirX*dirX + dirY*dirY + dirZ*dirZ);
    dirX /= length; dirY /= length; dirZ /= length;

    // --- Metszésvizsgálat a 4 cserép képzeletbeli gömbjével ---
    double bounding_radius = 2.0; // A cserép befoglaló sugara (állítsd be a modeled méretétől függően)

    for (int i = 0; i < 4; i++) {
        app->scene.pots[i].is_selected = false; // Töröljük az előző kijelöléseket

        // Vektor a sugár kezdetétől a cserép középpontjáig
        double ocX = posX1 - app->scene.pots[i].x;
        double ocY = posY1 - 0.0; // A cserép Y pozíciója 0
        double ocZ = posZ1 - app->scene.pots[i].z;

        // Másodfokú egyenlet a sugár-gömb metszésre
        double a = dirX*dirX + dirY*dirY + dirZ*dirZ; // Ez 1.0
        double b = 2.0 * (ocX*dirX + ocY*dirY + ocZ*dirZ);
        double c = (ocX*ocX + ocY*ocY + ocZ*ocZ) - (bounding_radius * bounding_radius);

        double discriminant = b*b - 4*a*c;

        // Ha a diszkrimináns > 0, akkor a sugár eltalálta a cserepet!
        if (discriminant > 0) {
            app->scene.pots[i].is_selected = true;
            printf("Cserep %d kijelolve!\n", i+1);
        }
    }
}

void init_app(App* app, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    app->window = SDL_CreateWindow("Feleves Feladat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    app->gl_context = SDL_GL_CreateContext(app->window);

    // OpenGL beállítások
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Égbolt színe

    // Projekció beállítása
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)width / (double)height, 0.1, 1000.0);

    // Egér elrejtése és ablakhoz rögzítése a forgatáshoz
    SDL_SetRelativeMouseMode(SDL_TRUE);

    init_camera(&(app->camera));
    init_scene(&(app->scene));

    app->is_running = true;
    app->uptime = SDL_GetTicks() / 1000.0;
}

void handle_app_events(App* app)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                app->is_running = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Bal klikk esetén meghívjuk a raycastingot az egér X, Y koordinátáival
                    perform_raycast(app, event.button.x, event.button.y);
                }
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_1 || event.key.keysym.sym == SDLK_2 || event.key.keysym.sym == SDLK_0) {
                    int plant_to_set = 0;
                    if (event.key.keysym.sym == SDLK_1) plant_to_set = 1;
                    if (event.key.keysym.sym == SDLK_2) plant_to_set = 2;

                    for (int i = 0; i < 4; i++) {
                        if (app->scene.pots[i].is_selected) {
                            app->scene.pots[i].active_plant = plant_to_set;
                        }
                    }
                }
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE: app->is_running = false; break;
                    case SDL_SCANCODE_W: app->camera.is_moving_forward = true; break;
                    case SDL_SCANCODE_S: app->camera.is_moving_backward = true; break;
                    case SDL_SCANCODE_A: app->camera.is_moving_left = true; break;
                    case SDL_SCANCODE_D: app->camera.is_moving_right = true; break;
                    case SDL_SCANCODE_F1: 
                        app->show_help = !app->show_help; // Súgó ki-be kapcsolása 
                        break;

                    // FÉNYERŐ NÖVELÉSE: Numpad + VAGY PageUp
                    case SDL_SCANCODE_KP_PLUS:
                    case SDL_SCANCODE_PAGEUP:
                        app->scene.light_intensity += 0.1f;
                        if (app->scene.light_intensity > 1.2f) app->scene.light_intensity = 1.2f;
                        //printf("Fenyero novelve: %.2f\n", app->scene.light_intensity);
                        break;
                    
                    // FÉNYERŐ CSÖKKENTÉSE: Numpad - VAGY PageDown
                    case SDL_SCANCODE_KP_MINUS:
                    case SDL_SCANCODE_PAGEDOWN:
                        app->scene.light_intensity -= 0.1f;
                        if (app->scene.light_intensity < 0.0f) app->scene.light_intensity = 0.0f;
                        //printf("Fenyero csokkentve: %.2f\n", app->scene.light_intensity);
                        break;
                    case SDL_SCANCODE_R: // R gomb megnyomására elindul az öntözés
                        app->scene.watering_timer = 10.0; // 10 másodpercig tartson
                        printf("Ontozorendszer bekapcsolva!\n");
                        break;
                    default: break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W: app->camera.is_moving_forward = false; break;
                    case SDL_SCANCODE_S: app->camera.is_moving_backward = false; break;
                    case SDL_SCANCODE_A: app->camera.is_moving_left = false; break;
                    case SDL_SCANCODE_D: app->camera.is_moving_right = false; break;
                    default: break;
                }
                break;
            case SDL_MOUSEMOTION:
                // Egér jobbra (+xrel)
                app->camera.rotation[1] += event.motion.xrel * 0.1; 
                // Egér le (+yrel)
                app->camera.rotation[0] += event.motion.yrel * 0.1;

                // Függőleges korlát
                if (app->camera.rotation[0] > 90.0) app->camera.rotation[0] = 90.0;
                if (app->camera.rotation[0] < -90.0) app->camera.rotation[0] = -90.0;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    // Ablak átméretezésének kezelése
                    glViewport(0, 0, event.window.data1, event.window.data2);
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    gluPerspective(60.0, (double)event.window.data1 / (double)event.window.data2, 0.1, 1000.0);
                }
                break;
        }
    }
}

void update_app(App* app)
{
    double current_time = SDL_GetTicks() / 1000.0;
    double dt = current_time - app->uptime;
    app->uptime = current_time;

    update_camera(&(app->camera), dt);
    update_scene(&(app->scene), dt);
}

void render_app(App* app)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_view(&(app->camera));
    render_scene(&(app->scene));
    if (app->show_help) {
        render_help(&(app->scene));
    }

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app)
{
    SDL_GL_DeleteContext(app->gl_context);
    SDL_DestroyWindow(app->window);
    IMG_Quit();
    SDL_Quit();
}