#include <main.h>

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
    feed.build_feed<false>(GTK_LIST_BOX(listbox_right));

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

// Function to parse and process the RSS feed
template <bool REFRESH>
void feed_ui_t::build_feed(GtkListBox *listbox)
{
    mrss_error_t err;
    mrss_t *root = NULL;
    mrss_item_t *channel = NULL, *item = NULL;

    if ((err = mrss_parse_url(const_cast<char*>(url.c_str()), &root)) != MRSS_OK)
    {
        fprintf(stderr, "Error while parsing url: %s\n", mrss_strerror(err));
        return;
    }
    
    // If it's a refresh call, do not allocate further memory
    if constexpr (!REFRESH)
    {
            
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
        gtk_list_box_insert(GTK_LIST_BOX(listbox), title_row, 0);
    }

    int i = 1;
    GtkListBoxRow *row = NULL;
    while ((row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(listbox), i)) != NULL)
        gtk_widget_destroy((GtkWidget*)row);

    items.erase(items.begin(), items.end());

    int stop = 0;
    for (item = root->item; item != NULL && stop < BUILD_FEED_MAX_ENTRIES; item = item->next, stop++)
    {
        char *display_text = g_strdup_printf("%s\n%s", item->title
                                                     , item->link);

        // Store items
        this->items.push_back({(item->title != NULL) ? item->title : "(None)",
                                    (item->link != NULL) ? item->link : "(None)",
                                    (item->description != NULL) ? item->description : "(None)"});

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

    // Free root element of the XML
    mrss_free(root);
}

/*
void feed_ui_t::set_refresh_timer(std::size_t millis)
{
    if (this->timer == 0)
    {
        g_timeout_add(millis, refresh_feed, (gpointer) this);
    }
}

void feed_ui_t::remove_refresh_timer()
{
    if (this->timer != 0)
    {
        g_source_remove(this->timer);
        timer = 0;
    }
}
*/
/*
void feed_ui_t::reset_feed()
{
    // Remove timer
    this->remove_refresh_timer();

    // Destroy the current listbox
    gtk_widget_destroy(listbox);
    listbox = NULL;

    items.erase(items.begin(), items.end());
}
*/

// Intermediate wrapper that for build feed
gboolean refresh_feed(gpointer data)
{
    window_ui_t *wnd = (window_ui_t*)data;

    wnd->feeds[wnd->current_feed_index].build_feed<true>(GTK_LIST_BOX(wnd->listbox_right));

    gtk_widget_show_all(wnd->scrolledwindow_right);

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
    auto sources = read_sources("../sources.txt");

    // Initialize window
    window_ui_t window(sources);

    gtk_main();

    return 0;
}
