#include "app.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

void init_app(App* app, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    app->window = SDL_CreateWindow("Feieves Feladat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE: app->is_running = false; break;
                    case SDL_SCANCODE_W: app->camera.is_moving_forward = true; break;
                    case SDL_SCANCODE_S: app->camera.is_moving_backward = true; break;
                    case SDL_SCANCODE_A: app->camera.is_moving_left = true; break;
                    case SDL_SCANCODE_D: app->camera.is_moving_right = true; break;
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
                // Egér mozgás alapján kamera forgatása
                app->camera.rotation[1] -= event.motion.xrel * 0.2; // Y tengely (bal/jobb)
                app->camera.rotation[0] -= event.motion.yrel * 0.2; // X tengely (fel/le nézés)
                
                // Korlátozzuk a fel/le nézést, hogy ne forduljon át a kamera
                if (app->camera.rotation[0] > 89.0) app->camera.rotation[0] = 89.0;
                if (app->camera.rotation[0] < -89.0) app->camera.rotation[0] = -89.0;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    // Ablak átméretezésének kezelése (ez elvárás!)
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
}

void render_app(App* app)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_view(&(app->camera));
    render_scene(&(app->scene));

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app)
{
    SDL_GL_DeleteContext(app->gl_context);
    SDL_DestroyWindow(app->window);
    IMG_Quit();
    SDL_Quit();
}