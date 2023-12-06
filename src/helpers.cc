#include <helpers.h>
#include <window_ui.h>

std::string strip_html(std::string_view old_str)
{
    std::string new_str; new_str.reserve(old_str.size());

    int tag = 0;
    for (char c : old_str)
    {
        tag = (c == '<') ? tag+1 : (c == '>') ? tag-1 : tag;

        if (tag == 0 && c != '>')
            new_str.push_back(c);
    }

    new_str.shrink_to_fit();
    return new_str;
}

// Intermediate wrapper that for build feed
gboolean refresh_feed(gpointer data)
{
    window_ui_t *wnd = (window_ui_t*)data;

    wnd->feeds[wnd->current_feed_index].build_feed<true>(GTK_LIST_BOX(wnd->listbox_right));

    gtk_widget_show_all(wnd->scrolledwindow_right);

    // G_SOURCE_CONTINUE keeps the timer running
    return G_SOURCE_CONTINUE;
}