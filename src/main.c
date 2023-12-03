#include <gtk/gtk.h>
#include <mrss.h>
#include <string.h>
#include <events.h>
#include <stdbool.h>

// Function to parse and process the RSS feed
void build_feed(const char *url, GtkWidget *listbox)
{
    mrss_error_t err;
    mrss_t *root = NULL;
    mrss_item_t *channel = NULL, *item = NULL;

    if ((err = mrss_parse_url(url, &root)) != MRSS_OK)
    {
        fprintf(stderr, "Error while parsing url: %s\n", mrss_strerror(err));
        return;
    }

    channel = root->item;
    
    if (channel == NULL)
    {
        fprintf(stderr, "No items in root element!\n");
        return;
    }

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
        gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);

        // Free allocated memory
        g_free(display_text);
    }

    mrss_free(root);
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "minRSS");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Create a vertical box layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a listbox to display the RSS items
    GtkWidget *listbox = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), listbox, TRUE, TRUE, 0);

    build_feed("https://www.amren.com/category/podcasts/feed", listbox);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
