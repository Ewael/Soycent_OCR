#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <gtk/gtk.h>

struct UiTrain {
    GtkWindow* window;

    GtkButton* next_btn;
    GtkButton* cancel_btn;

    GtkStack* stack;

    GtkFileChooserWidget* file_chooser;

    GtkBox* firststep;
    GtkBox* filestep;
    GtkBox* trainstep;
    GtkBox* donestep;

    int currentstep;
};

typedef struct {
    /* VAR DEFINITION */
    GtkBuilder *builder;

    GtkWindow* main_window; // main window

    GtkButton* about_button; // about menu button
    GtkButton* open_button;
    GtkButton* save_button;
    GtkButton* train_button;

    GtkBox* start_page;
    GtkBox* main_box;

    GtkTextBuffer* main_text_buffer; // main text buffer

    GtkStack* main_stack;

    struct UiTrain uitrain;

} UiApp;


UiApp create_ui(GtkBuilder *builder);

void on_open_btn_pressed(GtkButton *button, gpointer user_data); 

int save_text_buffer(char *s, char *path);

void on_save_btn_pressed(GtkButton *button, gpointer user_data);

void on_about_btn_pressed(GtkButton *button, gpointer user_data);

#endif
