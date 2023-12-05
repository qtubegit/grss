#include <events.h>
#include <main.h>

gboolean on_key_press(GtkWidget *_, GdkEventKey *event, gpointer data)
{
    if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_k)
    {
        window_ui_t *wnd = reinterpret_cast<window_ui_t*>(data);

        auto& old_feed = wnd->feeds[wnd->current_feed_index];
        
        // Remove listbox from scrollview
        gtk_container_remove(GTK_CONTAINER(wnd->scrolledwindow_right), old_feed.listbox);

        // Reset feed
        old_feed.reset_feed();

        // Increase the index
        wnd->current_feed_index = (wnd->current_feed_index+1) % wnd->feeds.size();

        // Attach new listbox
        auto& new_feed = wnd->feeds[wnd->current_feed_index];
        new_feed.build_feed<false>();
        new_feed.set_refresh_timer(60000);
        gtk_container_add(GTK_CONTAINER(wnd->scrolledwindow_right), new_feed.listbox);

        gtk_widget_show_all(wnd->window);
        
        // Case is handled
        return TRUE;
    }

    // Default behavior
    return FALSE;
}

gboolean on_row_activated(GtkWidget *listbox, GtkWidget *row, gpointer data)
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

        return TRUE;
    }

    return FALSE;
}

void on_window_destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}