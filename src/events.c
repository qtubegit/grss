#include <events.h>

void on_window_destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}