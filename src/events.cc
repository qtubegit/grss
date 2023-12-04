#include <events.h>
#include <main.h>

void on_row_activated(GtkWidget *listbox, GtkWidget *row, gpointer data)
{
    feed_ui_t *feed = reinterpret_cast<feed_ui_t*>(data);
    gint index = gtk_list_box_row_get_index(GTK_LIST_BOX_ROW(row));

    if (index > 0)
    {
        const char* url = feed->items[index-1].link.c_str();
        
        #ifdef _WIN32
            // Windows
            ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
        #elif __APPLE__
            // macOS
            char command[255];
            sprintf(command, "open %s", url);
            system(command);
        #elif __linux__
            // Linux
            char command[255];
            sprintf(command, "xdg-open %s", url);
            system(command);
        #else
            #error Unsupported operating system
        #endif
    }
}

void on_window_destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}