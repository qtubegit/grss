#ifndef _WINDOW_UI_H
#define _WINDOW_UI_H

#include <gtk/gtk.h>
#include <mrss.h>
#include <string.h>
#include <string>
#include <stdbool.h>
#include <vector>
#include <type_traits>
#include <feed.h>

struct window_ui_t
{
    GtkWidget *window;
    GtkWidget *hbox;
    GtkWidget *scrolledwindow_left;
    GtkWidget *scrolledwindow_right;
    GtkWidget *listbox_right;
    GtkWidget *channel_list;
    guint timer;

    std::size_t current_feed_index;
    std::vector<feed_ui_t> feeds;

    window_ui_t(const std::vector<std::string>& source_list);

    ~window_ui_t()
    {
        g_source_remove(timer);
    }
};

#endif