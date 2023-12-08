#ifndef _HELPERS_H
#define _HELPERS_H

#include <gtk/gtk.h>
#include <string>
#include <filesystem>
#include <curl/curl.h>

struct image_downloader_t
{
    CURL *curl;

    image_downloader_t() : curl{curl_easy_init()} {}
    ~image_downloader_t()
    {
        curl_easy_cleanup(curl);
    }


    void download_image(std::string_view, const std::filesystem::path&);
};

std::string strip_html(std::string_view);
gboolean refresh_feed(gpointer);

#endif