#include "noteapp.hpp"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    NoteApp *app = NoteApp::getInstance();
    app->run();
    return 0;
}
