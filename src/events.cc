#include <events.h>
#include <main.h>

static void switch_feed(window_ui_t *wnd, std::size_t feed_index) 
{
    auto& old_feed = wnd->feeds[wnd->current_feed_index];

    // Increase the index
    wnd->current_feed_index = feed_index;

    // Attach new listbox
    auto& new_feed = wnd->feeds[feed_index];
    new_feed.build_feed<false>(GTK_LIST_BOX(wnd->listbox_right));

    // Select new row in the left scroll bar
    gtk_list_box_select_row(GTK_LIST_BOX(wnd->channel_list),
        gtk_list_box_get_row_at_index(GTK_LIST_BOX(wnd->channel_list), wnd->current_feed_index));

    gtk_widget_show_all(wnd->window);
}

gboolean on_key_press(GtkWidget *_, GdkEventKey *event, gpointer data)
{
    if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_k)
    {
        window_ui_t *wnd = reinterpret_cast<window_ui_t*>(data);
        std::size_t new_index = (wnd->current_feed_index+1) % wnd->feeds.size();
        switch_feed(wnd, new_index);
        
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

gboolean on_channel_row_activated(GtkWidget *listbox, GtkWidget *row, gpointer data)
{
    window_ui_t *wnd = reinterpret_cast<window_ui_t*>(data);
    gint index = gtk_list_box_row_get_index(GTK_LIST_BOX_ROW(row));
    switch_feed(wnd, index);
    return TRUE;
}

void on_window_destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}