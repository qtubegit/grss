#include <helpers.h>
#include <window_ui.h>

// Intermediate wrapper that for build feed
gboolean refresh_feed(gpointer data)
{
    window_ui_t *wnd = (window_ui_t*)data;

    wnd->feeds[wnd->current_feed_index].build_feed<true>(GTK_LIST_BOX(wnd->listbox_right));

    gtk_widget_show_all(wnd->scrolledwindow_right);

    // G_SOURCE_CONTINUE keeps the timer running
    return G_SOURCE_CONTINUE;
}