#include <fstream>
#include <stdio.h>
#include <events.h>
#include <feed.h>
#include <helpers.h>
#include <window_ui.h>

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
