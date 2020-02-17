#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "ui/main.h"
#include "ui/train.h"

int main() {
    gtk_init(NULL, NULL);

    GtkBuilder* builder = gtk_builder_new();

    GError* error = NULL;

    if (gtk_builder_add_from_file(builder, "main.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    UiApp uiapp = create_ui(builder);

    gtk_widget_set_sensitive(GTK_WIDGET(uiapp.save_button), FALSE);

    /* START OF SIGNAL CONNECT */
    g_signal_connect(uiapp.main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(uiapp.about_button, "pressed", G_CALLBACK(on_about_btn_pressed), &uiapp);
    g_signal_connect(uiapp.open_button, "pressed", G_CALLBACK(on_open_btn_pressed), &uiapp);
    g_signal_connect(uiapp.train_button, "pressed", G_CALLBACK(on_train_btn_pressed), &uiapp);
    g_signal_connect(uiapp.save_button, "pressed", G_CALLBACK(on_save_btn_pressed), &uiapp);
    g_signal_connect(uiapp.uitrain.cancel_btn, "pressed", G_CALLBACK(on_train_cancel_pressed), &uiapp);
    g_signal_connect(uiapp.uitrain.next_btn, "pressed", G_CALLBACK(on_train_next_pressed), &uiapp);
    g_signal_connect(uiapp.uitrain.window, "delete-event", G_CALLBACK(on_train_close), &uiapp);
    /* END OF SIGNAL CONNECT */

    gtk_main();

    return 0;
}
