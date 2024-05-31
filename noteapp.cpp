#include "noteapp.hpp"

NoteApp* NoteApp::instance = nullptr;

NoteApp* NoteApp::getInstance() {
    if (instance == nullptr) {
        instance = new NoteApp();
    }
    return instance;
}

NoteApp::NoteApp() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Note App");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *add_button = gtk_button_new_with_label("Add Note");
    Command *addCommand = new AddNoteCommand(this);
    g_signal_connect_swapped(add_button, "clicked", G_CALLBACK(&_command_execute), addCommand);
    gtk_box_pack_start(GTK_BOX(hbox), add_button, TRUE, TRUE, 0);

    GtkWidget *delete_button = gtk_button_new_with_label("Delete Note");
    Command *deleteCommand = new DeleteNoteCommand(this);
    g_signal_connect_swapped(delete_button, "clicked", G_CALLBACK(&_command_execute), deleteCommand);
    gtk_box_pack_start(GTK_BOX(hbox), delete_button, TRUE, TRUE, 0);

    GtkWidget *edit_button = gtk_button_new_with_label("Edit Note");
    Command *editCommand = new EditNoteCommand(this);
    g_signal_connect_swapped(edit_button, "clicked", G_CALLBACK(&_command_execute), editCommand);
    gtk_box_pack_start(GTK_BOX(hbox), edit_button, TRUE, TRUE, 0);

    GtkWidget *save_button = gtk_button_new_with_label("Save Notes");
    Command *saveCommand = new SaveNoteCommand(this);
    g_signal_connect_swapped(save_button, "clicked", G_CALLBACK(&_command_execute), saveCommand);
    gtk_box_pack_start(GTK_BOX(hbox), save_button, TRUE, TRUE, 0);

    GtkWidget *test_button = gtk_button_new_with_label("Test Notification");
    Command *testCommand = new TestNotificationCommand();
    g_signal_connect_swapped(test_button, "clicked", G_CALLBACK(&_command_execute), testCommand);
    gtk_box_pack_start(GTK_BOX(hbox), test_button, TRUE, TRUE, 0);

    list_box = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), list_box, TRUE, TRUE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    noteSaverProxy = new NoteSaverProxy(this);
    load_notes();
}

void NoteApp::run() {
    gtk_widget_show_all(window);
    gtk_main();
}

void NoteApp::add_note_with_time(const gchar *time_text) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (text && *text) {
        Note note;
        note.text = text;
        note.time = time_text;
        note.timestamp = get_current_time();
        notes.push_back(note);
        update_list_box();
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    }
}

void NoteApp::add_note() {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (text && *text) {
        Note note;
        std::string full_text = text;
        size_t pos = full_text.find(".");
        if (pos != std::string::npos) {
            note.text = full_text.substr(0, pos);
            note.time = full_text.substr(pos + 1);
        } else {
            note.text = full_text;
            note.time = "";
        }
        note.timestamp = get_current_time();
        notes.push_back(note);
        update_list_box();
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    }
}

void NoteApp::delete_note() {
    GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list_box));
    if (row) {
        int index = gtk_list_box_row_get_index(row);
        gtk_widget_destroy(GTK_WIDGET(row));
        notes.erase(notes.begin() + index);
        update_list_box();
    }
}

void NoteApp::edit_note() {
    GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list_box));
    if (row) {
        int index = gtk_list_box_row_get_index(row);
        const gchar *new_text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (new_text && *new_text) {
            notes[index].text = new_text;
            update_list_box();
            gtk_entry_set_text(GTK_ENTRY(entry), "");
        }
    }
}

void NoteApp::save_notes() {
    noteSaverProxy->save_notes(notes);
}

void NoteApp::load_notes() {
    std::ifstream file("notes.txt");
    notes.clear();
    std::string text;
    std::string timestamp;
    std::string time;
    while (std::getline(file, text) && std::getline(file, timestamp) && std::getline(file, time)) {
        Note note = {text, timestamp, time};
        notes.push_back(note);
    }
    update_list_box();
}

std::string NoteApp::get_current_time() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void NoteApp::update_list_box() {
    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for (GList *iter = children; iter != nullptr; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    for (size_t i = 0; i < notes.size(); ++i) {
        std::ostringstream oss;
        oss << i + 1 << ". " << notes[i].text << " (Created at: " << notes[i].timestamp;
        if (!notes[i].time.empty()) {
            oss << ", Time: " << notes[i].time;
        }
        oss << ")";
        GtkWidget *label = gtk_label_new(oss.str().c_str());
        gtk_list_box_insert(GTK_LIST_BOX(list_box), label, -1);
    }
    gtk_widget_show_all(list_box);
}

void NoteApp::display_notification(const std::string& message) {
    MessageBoxA(NULL, message.c_str(), "Notification", MB_OK | MB_ICONINFORMATION);
}

extern "C" void _command_execute(gpointer data) {
    Command *command = static_cast<Command*>(data);
    command->execute();
}

// Реализация классов команд
AddNoteCommand::AddNoteCommand(NoteApp* app) : app(app) {}

void AddNoteCommand::execute() {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(app->entry));
    if (text && *text) {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
                "Enter Time",
                GTK_WINDOW(app->window), GTK_DIALOG_MODAL,
                ("OK"), GTK_RESPONSE_OK, ("Cancel"), GTK_RESPONSE_CANCEL,
                NULL);
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *time_entry = gtk_entry_new();
        gtk_container_add(GTK_CONTAINER(content_area), time_entry);
        gtk_widget_show_all(dialog);
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_OK) {
            const gchar *time_text = gtk_entry_get_text(GTK_ENTRY(time_entry));
            if (time_text && *time_text) {
                app->add_note_with_time(time_text);
            }
        }
        gtk_widget_destroy(dialog);
    }
}

DeleteNoteCommand::DeleteNoteCommand(NoteApp* app) : app(app) {}

void DeleteNoteCommand::execute() {
    app->delete_note();
}

EditNoteCommand::EditNoteCommand(NoteApp* app) : app(app) {}

void EditNoteCommand::execute() {
    app->edit_note();
}

SaveNoteCommand::SaveNoteCommand(NoteApp* app) : app(app) {}

void SaveNoteCommand::execute() {
    app->save_notes();
}

void TestNotificationCommand::execute() {
    NoteApp::display_notification("This is a test notification from NoteApp.");
}

NoteSaverProxy::NoteSaverProxy(NoteApp* app) : app(app) {}

void NoteSaverProxy::save_notes(const std::vector<Note>& notes) {
    std::ofstream file("notes.txt");
    for (const auto &note : notes) {
        file << note.text << "\n" << note.timestamp << "\n" << note.time << "\n";
    }
}
