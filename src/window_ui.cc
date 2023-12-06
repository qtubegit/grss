#include <feed.h>
#include <window_ui.h>

// Constructor for window_ui_t
window_ui_t::window_ui_t(const std::vector<std::string>& source_list)
{
    // Set up window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gRSS");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Create a horizontal box layout
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), hbox);
    
    scrolledwindow_left = gtk_scrolled_window_new(NULL, NULL);
    scrolledwindow_right = gtk_scrolled_window_new(NULL, NULL);
    //box_invisible = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_left, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_right, TRUE, TRUE, 0);

    // Channel list
    channel_list = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolledwindow_left), channel_list);
    g_signal_connect(channel_list, "row-activated", G_CALLBACK(on_channel_row_activated), (gpointer)this);

    // Feeds loop (Build Multiple Feeds)
    for (const auto& source : source_list) {
        std::string delimiter = ",";
        std::size_t d1 = source.find(delimiter);
        std::string title = source.substr(0, d1);
        std::string url = source.substr(d1 + 1, source.length());

        feeds.push_back(feed_ui_t(url));

        // Channel list
        GtkWidget *channel_row = gtk_list_box_row_new();
        GtkWidget *channel_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        GtkWidget *channel_label = gtk_label_new(title.c_str());

        gtk_label_set_xalign(GTK_LABEL(channel_label), 0.0f);
        gtk_label_set_line_wrap(GTK_LABEL(channel_label), TRUE);
        gtk_container_add(GTK_CONTAINER(channel_row), channel_box);
        gtk_box_pack_start(GTK_BOX(channel_box), channel_label, FALSE, FALSE, 0);
        gtk_list_box_insert(GTK_LIST_BOX(channel_list), channel_row, -1);
    }

    // Set the last-row as selected
    gtk_list_box_select_row(GTK_LIST_BOX(channel_list),
        gtk_list_box_get_row_at_index(GTK_LIST_BOX(channel_list), feeds.size()-1));

    // Build listbox and feed
    listbox_right = gtk_list_box_new();
    auto& feed = feeds.back();
    feed.build_feed<true>(GTK_LIST_BOX(listbox_right));

    // Add refresh timer
    timer = g_timeout_add(60000, refresh_feed, (gpointer) this);

    // Add listbox_right signal handler
    g_signal_connect(listbox_right, "row-activated", G_CALLBACK(on_row_activated), (gpointer) this);

    // Attach CNTRL-K handler
    g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_press), (gpointer) this);

    // Show feed
    gtk_container_add(GTK_CONTAINER(scrolledwindow_right), listbox_right);
    current_feed_index = feeds.size()-1;

    gtk_widget_show_all(window);
}