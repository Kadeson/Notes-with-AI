#ifndef NOTEAPP_HPP
#define NOTEAPP_HPP

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ctime>
#include <Windows.h>

struct Note {
    std::string text;
    std::string timestamp;
    std::string time;
};

class NoteSaverProxy;

class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
};

extern "C" void _command_execute(gpointer data);

class NoteApp {
public:
    static NoteApp* getInstance();
    void run();
    void add_note();
    void delete_note();
    void edit_note();
    void save_notes();
    void add_note_with_time(const gchar *time_text);
    std::string get_current_time();
    void update_list_box();
    static void display_notification(const std::string& message);

    GtkWidget *window;
    GtkWidget *entry;
    GtkWidget *list_box;
    std::vector<Note> notes;

private:
    NoteApp();
    void load_notes();
    NoteSaverProxy* noteSaverProxy;
    static NoteApp* instance;
};

class NoteSaverProxy {
public:
    NoteSaverProxy(NoteApp* app);
    void save_notes(const std::vector<Note>& notes);

private:
    NoteApp* app;
};

class AddNoteCommand : public Command {
public:
    AddNoteCommand(NoteApp* app);
    void execute() override;

private:
    NoteApp* app;
};

class DeleteNoteCommand : public Command {
public:
    DeleteNoteCommand(NoteApp* app);
    void execute() override;

private:
    NoteApp* app;
};

class EditNoteCommand : public Command {
public:
    EditNoteCommand(NoteApp* app);
    void execute() override;

private:
    NoteApp* app;
};

class SaveNoteCommand : public Command {
public:
    SaveNoteCommand(NoteApp* app);
    void execute() override;

private:
    NoteApp* app;
};

class TestNotificationCommand : public Command {
public:
    void execute() override;
};

#endif // NOTEAPP_HPP
