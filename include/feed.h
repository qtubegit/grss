#ifndef _FEED_H
#define _FEED_H

#include <gtk/gtk.h>
#include <mrss.h>
#include <string.h>
#include <string>
#include <stdbool.h>
#include <vector>
#include <type_traits>
#include <events.h>
#include <helpers.h>

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
    std::string title;
    std::vector<feed_item_t> items;

    feed_ui_t(std::string_view _url) : url{_url}, title{"No Title"} {}

    template <bool REFRESH>
    void build_feed(GtkListBox *);
};


#endif