#ifndef _MAIN_H
#define _MAIN_H

#include <gtk/gtk.h>
#include <mrss.h>
#include <string.h>
#include <string>
#include <events.h>
#include <stdbool.h>
#include <vector>
#include <type_traits>
#include <fstream>

#define BUILD_FEED_MAX_ENTRIES 20

gboolean refresh_feed(gpointer data);

std::vector<std::string> read_sources(const std::string&);

struct feed_item_t
{
    std::string title;
    std::string link;
    std::string description;
};

struct feed_ui_t
{
    std::string url;
    GtkWidget *listbox;
    std::vector<feed_item_t> items;
    guint timer;

    feed_ui_t(std::string_view _url) 
        : url{_url}, timer{0}, listbox{NULL} {}

    template <bool REFRESH>
    void build_feed();

    void set_refresh_timer(std::size_t);

    void remove_refresh_timer();
    
    void reset_feed();
};

struct window_ui_t
{
    GtkWidget *window;
    GtkWidget *hbox;
    GtkWidget *scrolledwindow_left;
    GtkWidget *scrolledwindow_right;
    GtkWidget *channel_list;
    //GtkWidget *box_invisible;
    std::size_t current_feed_index;

    std::vector<feed_ui_t> feeds;

    window_ui_t(const std::vector<std::string>& source_list);
};

#endif