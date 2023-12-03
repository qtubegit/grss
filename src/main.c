#include <gtk/gtk.h>
#include <mrss.h>
#include <string.h>
#include <events.h>
#include <stdbool.h>

// Struct that packs build_feed arguments
typedef struct
{
    char *url;
    GtkWidget *listbox;
} feed_ui_t;

void build_feed(const char *url, GtkWidget *listbox);
gboolean refresh_feed(gpointer data);


// Intermediate wrapper that for build feed
gboolean refresh_feed(gpointer data)
{
    feed_ui_t *feed_data = (feed_ui_t *)data;

    build_feed(feed_data->url, feed_data->listbox);

    // G_SOURCE_CONTINUE keeps the timer running
    return G_SOURCE_CONTINUE;
}

// Function to parse and process the RSS feed
void build_feed(const char *url, GtkWidget *listbox)
{
    mrss_error_t err;
    mrss_t *root = NULL;
    mrss_item_t *channel = NULL, *item = NULL;

    gtk_container_foreach(GTK_CONTAINER(listbox), (GtkCallback)gtk_widget_destroy, NULL);

    if ((err = mrss_parse_url(url, &root)) != MRSS_OK)
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
        char *display_text = g_strdup_printf("%s\n%s\n%s", item->title
                                                        , item->link
                                                        , item->description);

        // Create a listbox row
        GtkWidget *row = gtk_label_new(display_text);
        gtk_label_set_line_wrap(GTK_LABEL(row), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(row), 20);
        gtk_label_set_xalign(GTK_LABEL(row), 0.0f);
        gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);

        // Free allocated memory
        g_free(display_text);
    }

    gtk_widget_show_all(listbox);
    mrss_free(root);
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gRSS");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Create a vertical box layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    GtkWidget *scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);

    // Create a listbox to display the RSS items
    GtkWidget *listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolledwindow), listbox);

    // Build feed and initialize timers for the subscribed-to feed
    feed_ui_t feed1 = {.url = "https://www.amren.com/category/podcasts/feed", .listbox = listbox};
    build_feed(feed1.url, feed1.listbox);
    guint timer_id = g_timeout_add(60000, refresh_feed, (gpointer) &feed1);

    gtk_widget_show_all(window);
    gtk_main();

    g_source_remove(timer_id);
    return 0;
}
