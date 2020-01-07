#ifndef UI_TRAIN_H
#define UI_TRAIN_H

#include <gtk/gtk.h>
#include "main.h"

void close_train_window(UiApp *uiapp);

void on_train_cancel_pressed(GtkButton *button, gpointer user_data); 

void on_train_next_pressed(GtkButton *button, gpointer user_data);

void on_train_btn_pressed(GtkButton *button, gpointer user_data);

gboolean on_train_close(GtkWidget *widget, GdkEvent *event, gpointer user_data);

#endif
