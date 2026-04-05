#include "app.h"
#include <GL/freeglut.h>
#include <locale.h>

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    setlocale(LC_NUMERIC, "C");

    App app;
    
    init_app(&app, 800, 600);

    while (app.is_running) {
        handle_app_events(&app);
        update_app(&app);
        render_app(&app);
    }

    destroy_app(&app);

    return 0;
}