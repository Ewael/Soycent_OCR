#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "train.h"
#include "../ocr.h"

UiApp create_ui(GtkBuilder *builder) {
    UiApp uiapp =
    {
        /* VAR SETTING */

        .builder = builder,
    
        .main_window = GTK_WINDOW(gtk_builder_get_object(builder, "org.soycent.ocr")),

        .about_button = GTK_BUTTON(gtk_builder_get_object(builder, "about_button")),

        .open_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_file_button")),

        .save_button = GTK_BUTTON(gtk_builder_get_object(builder, "save_button")),

        .train_button = GTK_BUTTON(gtk_builder_get_object(builder, "train_net_button")),
        
        .start_page = GTK_BOX(gtk_builder_get_object(builder, "start_page")),

        .main_box = GTK_BOX(gtk_builder_get_object(builder, "main_box")),

        .main_text_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "ocr_text_buffer")),

        .main_stack = GTK_STACK(gtk_builder_get_object(builder, "stack")),

        .uitrain = {
            .window = GTK_WINDOW(gtk_builder_get_object(builder, "train_window")),
            .next_btn = GTK_BUTTON(gtk_builder_get_object(builder, "train_next_button")),
            .cancel_btn = GTK_BUTTON(gtk_builder_get_object(builder, "train_cancel_button")),
            .stack = GTK_STACK(gtk_builder_get_object(builder, "train_stack")),
            .file_chooser = GTK_FILE_CHOOSER_WIDGET(gtk_builder_get_object(builder, "train_file_chooser")),
            .firststep = GTK_BOX(gtk_builder_get_object(builder, "train_first_step_box")),
            .filestep = GTK_BOX(gtk_builder_get_object(builder, "train_file_step_box")),
            .trainstep = GTK_BOX(gtk_builder_get_object(builder, "train_training_step_box")),
            .donestep = GTK_BOX(gtk_builder_get_object(builder, "train_done_step_box")),
        },

        /* END OF VAR SETTING */ 
    };

    return uiapp;
}

void on_open_btn_pressed(GtkButton *button, gpointer user_data) {
    (void)(button);
    UiApp *uiapp = user_data;
    GtkFileChooserDialog *file_dialog = GTK_FILE_CHOOSER_DIALOG(
            gtk_file_chooser_dialog_new("Open an image",
                uiapp->main_window,
                GTK_FILE_CHOOSER_ACTION_OPEN,
                "_Cancel",
                GTK_RESPONSE_CANCEL,
                "_Open",
                GTK_RESPONSE_ACCEPT,
                NULL));
    gint res = gtk_dialog_run(GTK_DIALOG(file_dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog));
        
        while(gtk_events_pending())
            gtk_main_iteration();
        
        gtk_widget_destroy(GTK_WIDGET(file_dialog));
        
        char *text_result = image_to_text(filename);

        g_free(filename);

        if (!text_result) {
            //error
            // @TODO: put some dialog idk
            g_print("error\n");
            return;
        }
        
        gtk_text_buffer_set_text(uiapp->main_text_buffer, text_result, strlen(text_result));
        g_free(text_result);
        gtk_stack_set_visible_child(uiapp->main_stack, GTK_WIDGET(uiapp->main_box));

        gtk_widget_set_sensitive(GTK_WIDGET(uiapp->save_button), TRUE);
        return;
    }

    gtk_widget_destroy(GTK_WIDGET(file_dialog));
}

int save_text_buffer(char *s, char *path) {
    FILE *fptr;
    fptr = fopen(path, "w");
    
    if (!fptr)
        return 1;

    fprintf(fptr, "%s\n", s);

    fclose(fptr);

    return 0;
}

void on_save_btn_pressed(GtkButton *button, gpointer user_data) {
    (void)(button);
    UiApp *uiapp = user_data;
    GtkFileChooserDialog *file_dialog = GTK_FILE_CHOOSER_DIALOG(
            gtk_file_chooser_dialog_new("Save text as",
                uiapp->main_window,
                GTK_FILE_CHOOSER_ACTION_SAVE,
                "_Cancel",
                GTK_RESPONSE_CANCEL,
                "_Save",
                GTK_RESPONSE_ACCEPT,
                NULL));
    
    gtk_file_chooser_set_do_overwrite_confirmation(
            GTK_FILE_CHOOSER(file_dialog), TRUE);

    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(file_dialog),
                                 "Untitled document");

    gint res = gtk_dialog_run(GTK_DIALOG(file_dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog));
        g_print("%s\n", filename);
        gtk_widget_destroy(GTK_WIDGET(file_dialog));

        GtkTextIter start;
        gtk_text_buffer_get_start_iter(uiapp->main_text_buffer, &start);

        GtkTextIter end;
        gtk_text_buffer_get_end_iter(uiapp->main_text_buffer, &end);

        if (save_text_buffer(gtk_text_buffer_get_text(uiapp->main_text_buffer,
                        &start, &end, FALSE), filename))
            g_print("error while saving.\n");

        g_free(filename);
    }

    gtk_widget_destroy(GTK_WIDGET(file_dialog));
}

void on_about_btn_pressed(GtkButton *button, gpointer user_data) {
    (void)(button);
    UiApp *uiapp = user_data;
    (void)(uiapp);
    GtkAboutDialog *res = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
    gtk_about_dialog_set_program_name(res, "Soycent OCR");
    gtk_about_dialog_set_version (res, "1.0");
    gtk_about_dialog_set_copyright (res, "Copyright © 2019 Soycent\nCopyright © 2019 EPITA");
    gtk_about_dialog_set_comments (res, "An optical character recognition tool.\nThis project was made for the EPITA engineering school by the student group Soycent.");
    gtk_about_dialog_set_license (res, "This software is property of the EPITA school. The source code of this softare cannot be used.");
    gtk_about_dialog_set_website (res, "https://git.kektus.xyz/soycent/ocr");
    gtk_about_dialog_set_website_label (res, "Visit the website");
    const gchar *authors[5] = { "Nicolas FROGER <nicolas.froger@epita.fr>",
        "Enzo BOURICHE <enzo.bouriche@epita.fr>",
        "Thomas BERLIOZ <thomas.berlioz@epita.fr>",
        "Corentin PAPE <corentin.pape@epita.fr>",
        NULL};
    gtk_about_dialog_set_authors (res, authors);
    gtk_about_dialog_set_logo_icon_name (res, "format-text-italic");
    gtk_window_set_modal (GTK_WINDOW(res), TRUE);
    gtk_window_set_keep_above(GTK_WINDOW(res), TRUE);
    gtk_dialog_run(GTK_DIALOG(res));
    gtk_widget_destroy(GTK_WIDGET(res));
}
