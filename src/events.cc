#include <events.h>

void on_row_activated(GtkWidget *listbox, GtkWidget *row)
{
    gint index = gtk_list_box_row_get_index(GTK_LIST_BOX_ROW(row));

    // Create a message dialog
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Element %d clicked!", index);

    // Run the dialog and destroy it when OK is clicked
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_window_destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}