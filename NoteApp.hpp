#ifndef NOTEAPP_HPP
#define NOTEAPP_HPP

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>

struct Note {
    std::string text;
    std::string timestamp;
    std::string time; // новое поле для времени
};

class NoteApp {
public:
    NoteApp();
    void run();

private:
    static void on_add_button_clicked(GtkWidget *widget, gpointer data);
    static void on_delete_button_clicked(GtkWidget *widget, gpointer data);
    static void on_edit_button_clicked(GtkWidget *widget, gpointer data);
    static void on_save_button_clicked(GtkWidget *widget, gpointer data);
    static void on_load_button_clicked(GtkWidget *widget, gpointer data);
    void add_note();
    void delete_note();
    void edit_note();
    void save_notes();
    void load_notes();
    std::string get_current_time();
    void update_list_box();

    GtkWidget *window;
    GtkWidget *entry;
    GtkWidget *list_box;
    std::vector<Note> notes;
};

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
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_button_clicked), this);
    gtk_box_pack_start(GTK_BOX(hbox), add_button, TRUE, TRUE, 0);

    GtkWidget *delete_button = gtk_button_new_with_label("Delete Note");
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), this);
    gtk_box_pack_start(GTK_BOX(hbox), delete_button, TRUE, TRUE, 0);

    GtkWidget *edit_button = gtk_button_new_with_label("Edit Note");
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_button_clicked), this);
    gtk_box_pack_start(GTK_BOX(hbox), edit_button, TRUE, TRUE, 0);

    GtkWidget *save_button = gtk_button_new_with_label("Save Notes");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), this);
    gtk_box_pack_start(GTK_BOX(hbox), save_button, TRUE, TRUE, 0);

    GtkWidget *load_button = gtk_button_new_with_label("Load Notes");
    g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_button_clicked), this);
    gtk_box_pack_start(GTK_BOX(hbox), load_button, TRUE, TRUE, 0);

    list_box = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), list_box, TRUE, TRUE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    load_notes(); // Загружаем заметки при запуске приложения
}

void NoteApp::run() {
    gtk_widget_show_all(window);
    gtk_main();
}

void NoteApp::on_add_button_clicked(GtkWidget *widget, gpointer data) {
    NoteApp *app = static_cast<NoteApp *>(data);
    app->add_note();
}

void NoteApp::on_delete_button_clicked(GtkWidget *widget, gpointer data) {
    NoteApp *app = static_cast<NoteApp *>(data);
    app->delete_note();
}

void NoteApp::on_edit_button_clicked(GtkWidget *widget, gpointer data) {
    NoteApp *app = static_cast<NoteApp *>(data);
    app->edit_note();
}

void NoteApp::on_save_button_clicked(GtkWidget *widget, gpointer data) {
    NoteApp *app = static_cast<NoteApp *>(data);
    app->save_notes();
}

void NoteApp::on_load_button_clicked(GtkWidget *widget, gpointer data) {
    NoteApp *app = static_cast<NoteApp *>(data);
    app->load_notes();
}

void NoteApp::add_note() {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (text && *text) {
        Note note;
        std::string full_text = text;
        size_t pos = full_text.find("."); // ищем точку, разделяющую заметку и время
        if (pos != std::string::npos) {
            note.text = full_text.substr(0, pos); // текст заметки - это все до точки
            note.time = full_text.substr(pos + 1); // время - это все после точки
        } else {
            note.text = full_text; // если точки нет, то весь текст - это заметка
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
    std::ofstream file("notes.txt");
    for (const auto &note : notes) {
        file << note.text << "\n" << note.timestamp << "\n";
    }
}

void NoteApp::load_notes() {
    std::ifstream file("notes.txt");
    notes.clear();
    std::string text;
    std::string timestamp;
    while (std::getline(file, text) && std::getline(file, timestamp)) {
        Note note = {text, timestamp};
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
        oss << i + 1 << ". " << notes[i].text << " (Created at: " << notes[i].timestamp << ", Time: " << notes[i].time << ")";
        GtkWidget *label = gtk_label_new(oss.str().c_str());
        gtk_list_box_insert(GTK_LIST_BOX(list_box), label, -1);
    }
    gtk_widget_show_all(list_box);
}

#endif // NOTEAPP_HPP
