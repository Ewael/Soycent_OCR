#include <gtk/gtk.h>
#include <stdlib.h>
#include "main.h"
#include "train.h"
#include "../ocr.h"

void close_train_window(UiApp *uiapp) {
    struct UiTrain *uitrain = &(uiapp->uitrain);
    gtk_stack_set_visible_child(uitrain->stack, GTK_WIDGET(uitrain->firststep));
    uitrain->currentstep = 0;
    gtk_window_close(uitrain->window);
}

void on_train_cancel_pressed(GtkButton *button, gpointer user_data) {
    UiApp *uiapp = user_data;
    (void)(button);
    close_train_window(uiapp);
}

void on_train_next_pressed(GtkButton *button, gpointer user_data) {
    (void)(button);
    UiApp *uiapp = user_data;
    struct UiTrain *uitrain = &(uiapp->uitrain);
    switch (uitrain->currentstep) {
        case 0: // welcome to filechoose
            gtk_stack_set_visible_child(uitrain->stack, GTK_WIDGET(uitrain->filestep));
            uitrain->currentstep++;
            break;
        case 1: // filechoose to train
            ; // this is not a mistake, leave semicolon here please
            char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(uitrain->file_chooser));
        
            
            if (filename && strlen(filename) != 0) {
                gtk_stack_set_visible_child(uitrain->stack, GTK_WIDGET(uitrain->trainstep));
                gtk_widget_set_visible(GTK_WIDGET(uitrain->cancel_btn), FALSE);
                uitrain->currentstep++;
                gtk_widget_set_visible(GTK_WIDGET(uitrain->next_btn), FALSE);
                gtk_button_set_label(uitrain->next_btn, "Close");
                while (gtk_events_pending())
                    gtk_main_iteration();
                start_training(filename);
                g_free(filename);
                gtk_stack_set_visible_child(uitrain->stack, GTK_WIDGET(uitrain->donestep));
                uitrain->currentstep++;
                gtk_widget_set_visible(GTK_WIDGET(uitrain->next_btn), TRUE);
            }

            break;
        case 2: //
            // not possible
            gtk_stack_set_visible_child(uitrain->stack, GTK_WIDGET(uitrain->donestep));
            uitrain->currentstep++;
            gtk_button_set_label(uitrain->next_btn, "Close");
            break;
        case 3: // done to close
            close_train_window(uiapp);
            break;
    }
}

void on_train_btn_pressed(GtkButton *button, gpointer user_data) {
    (void)(button);
    UiApp *uiapp = user_data;
    gtk_button_set_label(uiapp->uitrain.next_btn, "Next");
    gtk_widget_set_visible(GTK_WIDGET(uiapp->uitrain.cancel_btn), TRUE);
    uiapp->uitrain.currentstep = 0;
    gtk_stack_set_visible_child(uiapp->uitrain.stack, GTK_WIDGET(uiapp->uitrain.firststep));
    gtk_widget_show(GTK_WIDGET(uiapp->uitrain.window));
}

gboolean on_train_close(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    (void)(event);
    UiApp *uiapp = user_data;
    if (uiapp->uitrain.currentstep != 2) {
        gtk_widget_hide(widget);
    }
    return TRUE;
}

