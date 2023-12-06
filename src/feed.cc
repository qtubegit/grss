#include <feed.h>

// Function to parse and process the RSS feed
template <bool REFRESH>
void feed_ui_t::build_feed(GtkListBox *listbox)
{
    mrss_error_t err;
    mrss_t *root = NULL;
    mrss_item_t *channel = NULL, *item = NULL;
    
    // If it's a refresh call, do not allocate further memory
    if constexpr (REFRESH)
    {
        items.erase(items.begin(), items.end());
        if ((err = mrss_parse_url(const_cast<char*>(url.c_str()), &root)) != MRSS_OK)
        {
            fprintf(stderr, "Error while parsing url: %s\n", mrss_strerror(err));
            return;
        }

        if (root->title != NULL)
            title = root->title;
    }

    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Create and add a label for the title
    GtkWidget *title_label = gtk_label_new(title.c_str());
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
    gtk_list_box_insert(listbox, title_row, 0);

    int i = 1;
    GtkListBoxRow *row = NULL;
    while ((row = gtk_list_box_get_row_at_index(listbox, i)) != NULL)
        gtk_widget_destroy((GtkWidget*)row);


    if constexpr (REFRESH)
    {
        i = 0;
        for (item = root->item; item != NULL && i < BUILD_FEED_MAX_ENTRIES; item = item->next, i++)
        {
            // Store items
            items.push_back({(item->title != NULL) ? item->title : "(None)",
                                        (item->link != NULL) ? item->link : "(None)",
                                        (item->description != NULL) ? item->description : "(None)"});

            char *display_text = g_strdup_printf("%s\n%s", items.back().title.c_str()
                                                        , items.back().link.c_str());

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
            gtk_list_box_insert(listbox, item_row, -1);

            // Free allocated memory
            g_free(display_text);
        }

        // Free root element of the XML
        mrss_free(root);
    }
    else
    {
        for (const auto& item : items)
        {
            char *display_text = g_strdup_printf("%s\n%s", item.title.c_str()
                                                            , item.link.c_str());

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
    }
}

template void feed_ui_t::build_feed<true>(GtkListBox *);
template void feed_ui_t::build_feed<false>(GtkListBox *);