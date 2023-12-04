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

    feed_ui_t() : listbox{NULL} {}
    feed_ui_t(std::string_view _url, GtkWidget *_listbox) : url{_url}, listbox{_listbox} {}

    void build_feed();
};

struct window_ui_t
{
    GtkWidget *window;
    GtkWidget *webview;

    std::vector<feed_ui_t> feeds;
    std::vector<guint> timers;

    window_ui_t(std::vector<std::string> url_list)
    {
        // Set up window
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "gRSS");
        gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
        g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

        // Create a vertical box layout
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_container_add(GTK_CONTAINER(window), vbox);
        GtkWidget *scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
        gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);

        // Feeds loop (Build Multiple Feeds)
        for (const auto& url : url_list)
        {
            GtkWidget *listbox = gtk_list_box_new();
            feeds.push_back(feed_ui_t(url, listbox));

            auto& feed = feeds.back();
            feed.build_feed();

            // Add timer
            timers.push_back(g_timeout_add(60000, refresh_feed, (gpointer) &feed));

            // Add callback on row selection
            g_signal_connect(listbox, "row-activated", G_CALLBACK(on_row_activated), (gpointer) &feed);
        }

        // Show first feed on the window
        gtk_container_add(GTK_CONTAINER(scrolledwindow), feeds[0].listbox);

        gtk_widget_show_all(window);
    }

    ~window_ui_t()
    {
        for (auto timer : timers)
            g_source_remove(timer);
    }
};

#endif