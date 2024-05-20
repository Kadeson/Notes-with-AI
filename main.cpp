#include <gtk/gtk.h>
#include "NoteApp.hpp"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    NoteApp app;
    app.run();

    return 0;
}
