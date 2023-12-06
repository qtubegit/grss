#ifndef _EVENTS_H
#define _EVENTS_H

#include <gtk/gtk.h>
#include <string_view>

#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <stdlib.h>
    #include <unistd.h>
#elif __linux__
    #include <stdlib.h>
    #include <unistd.h>
#endif

gboolean on_key_press(GtkWidget*, GdkEventKey *, gpointer);
void on_window_destroy(GtkWidget*, gpointer);
gboolean on_row_activated(GtkWidget*, GtkWidget*, gpointer);
gboolean on_channel_row_activated(GtkWidget*, GtkWidget*, gpointer);

#endif