#include <helpers.h>
#include <window_ui.h>

void image_downloader_t::download_image(std::string_view url, const std::filesystem::path& filepath)
{
    CURLcode res;
    FILE *fp;

    fp = fopen(filepath.c_str(), "wb");

    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file for writing image\n");
        return;
    }

    // Set the URL to download
    curl_easy_setopt(curl, CURLOPT_URL, url.data());

    // Set the callback function to write data to the file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

    // Set the file stream as the write data
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    // Perform the request
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    fclose(fp);
}

std::string strip_html(std::string_view old_str)
{
    std::string new_str; new_str.reserve(old_str.size());

    int tag = 0;
    for (char c : old_str)
    {
        tag = (c == '<') ? tag+1 : (c == '>') ? tag-1 : tag;

        if (tag == 0 && c != '>')
            new_str.push_back(c);
    }

    new_str.shrink_to_fit();
    return new_str;
}

// Intermediate wrapper that for build feed
gboolean refresh_feed(gpointer data)
{
    window_ui_t *wnd = (window_ui_t*)data;

    wnd->feeds[wnd->current_feed_index].build_feed<true>(GTK_LIST_BOX(wnd->listbox_right));

    gtk_widget_show_all(wnd->scrolledwindow_right);

    // G_SOURCE_CONTINUE keeps the timer running
    return G_SOURCE_CONTINUE;
}