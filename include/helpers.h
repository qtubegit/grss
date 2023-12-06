#ifndef _HELPERS_H
#define _HELPERS_H

#include <gtk/gtk.h>
#include <string>

std::string strip_html(std::string_view);
gboolean refresh_feed(gpointer);

#endif