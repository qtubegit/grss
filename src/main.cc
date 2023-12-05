#include <main.h>

// Function to parse and process the RSS feed
void feed_ui_t::build_feed()
{
    mrss_error_t err;
    mrss_t *root = NULL;
    mrss_item_t *channel = NULL, *item = NULL;

    gtk_container_foreach(GTK_CONTAINER(listbox), (GtkCallback)gtk_widget_destroy, NULL);
    gtk_container_foreach(GTK_CONTAINER(channel_list), (GtkCallback)gtk_widget_destroy, NULL);

    if ((err = mrss_parse_url(const_cast<char*>(url.c_str()), &root)) != MRSS_OK)
    {
        fprintf(stderr, "Error while parsing url: %s\n", mrss_strerror(err));
        return;
    }
    
    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Create and add a label for the title
    GtkWidget *title_label = gtk_label_new(root->title);
    gtk_label_set_line_wrap(GTK_LABEL(title_label), TRUE);
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
    gtk_list_box_insert(GTK_LIST_BOX(channel_list), title_row, -1);

    int stop = 0;
    for (item = root->item; item != NULL && stop < 5; item = item->next, ++stop)
    {
        char *display_text = g_strdup_printf("%s\n%s", item->title
                                                     , item->link);

        // Store items
        this->items.push_back({item->title, item->link, item->description});

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

std::vector<std::string> read_sources(const std::string& file)
{
    std::vector<std::string> sources;

    std::ifstream ss(file);

    // Check if the file is open
    if (!ss.is_open())
    {
        fprintf(stderr, "Error opening file: %s\n", file.c_str());
        throw std::string("Sources file not found!");
    }

    // Read and print each line
    std::string line;
    while (std::getline(ss, line)) {
        sources.push_back(line);
    }

    ss.close();
    return sources;
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Read urls from file
    auto sources = read_sources("sources.txt");

    // Initialize window
    window_ui_t window(sources);

    gtk_main();

    return 0;
}
