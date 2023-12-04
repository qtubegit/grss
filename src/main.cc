#include <gtk/gtk.h>
#include <mrss.h>
#include <string.h>
#include <string>
#include <events.h>
#include <stdbool.h>
#include <webkit2/webkit2.h>
#include <vector>
#include <type_traits>

gboolean refresh_feed(gpointer data);

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

    window_ui_t(std::initializer_list<std::string_view> url_list)
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
        for (auto url : url_list)
        {
            GtkWidget *listbox = gtk_list_box_new();
            feed_ui_t feed(url, listbox);
            feed.build_feed();
            feeds.push_back(feed);
            timers.push_back(g_timeout_add(60000, refresh_feed, (gpointer) &feeds.back()));
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

// Function to parse and process the RSS feed
void feed_ui_t::build_feed()
{
    mrss_error_t err;
    mrss_t *root = NULL;
    mrss_item_t *channel = NULL, *item = NULL;

    gtk_container_foreach(GTK_CONTAINER(listbox), (GtkCallback)gtk_widget_destroy, NULL);

    if ((err = mrss_parse_url(const_cast<char*>(url.c_str()), &root)) != MRSS_OK)
    {
        fprintf(stderr, "Error while parsing url: %s\n", mrss_strerror(err));
        return;
    }
    
    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Create and add a label for the title
    GtkWidget *title_label = gtk_label_new(root->title);
    gtk_box_pack_start(GTK_BOX(title_box), title_label, FALSE, FALSE, 0);

    // Add additional spacing at the bottom
    GtkWidget *spacer = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(title_box), spacer, FALSE, FALSE, 10);

    // Create a GtkListBoxRow for the title
    GtkWidget *title_row = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(title_row), title_box);

    // Make the title row non-selectable
    gtk_list_box_row_set_selectable(GTK_LIST_BOX_ROW(title_row), FALSE);

    // Insert title into listbox
    gtk_list_box_insert(GTK_LIST_BOX(listbox), title_row, -1);

    int stop = 0;
    for (item = root->item; item != NULL && stop < 5; item = item->next, ++stop)
    {
        char *display_text = g_strdup_printf("%s\n%s", item->title
                                                        , item->link);

        // Store items
        this->items.push_back({.title = item->title, .link = item->link, .description = item->description});

        // Create ItemBox
        GtkWidget *item_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

        // Create and add label with title and link
        GtkWidget *item_label = gtk_label_new(display_text);
        gtk_label_set_line_wrap(GTK_LABEL(item_label), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(item_label), 20);
        gtk_label_set_xalign(GTK_LABEL(item_label), 0.0f);
        gtk_box_pack_start(GTK_BOX(item_box), item_label, FALSE, FALSE, 0);

        // Create separator
        GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start(GTK_BOX(item_box), separator, FALSE, FALSE, 0);

        // Create a row for the listbox
        GtkWidget *item_row = gtk_list_box_row_new();
        gtk_container_add(GTK_CONTAINER(item_row), item_box);

        // Insert row into listbox
        gtk_list_box_insert(GTK_LIST_BOX(listbox), item_row, -1);

        // Free allocated memory
        g_free(display_text);
    }

    // Show widgets (refresh)
    gtk_widget_show_all(listbox);

    // Add callback on row selection
    g_signal_connect(listbox, "row-activated", G_CALLBACK(on_row_activated), NULL);

    // Free root element of the XML
    mrss_free(root);
}

// Intermediate wrapper that for build feed
gboolean refresh_feed(gpointer data)
{
    feed_ui_t *feed = (feed_ui_t*)data;

    feed->build_feed();

    // G_SOURCE_CONTINUE keeps the timer running
    return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Initialize window
    window_ui_t window({"https://www.amren.com/category/podcasts/feed"});

    gtk_main();

    return 0;
}
