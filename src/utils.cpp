#include "utils.hpp"
#include <SDL3/SDL_log.h>
#include <fstream>
#include <string>

using std::fstream, std::string;

// TODO Read size of file to determine the max content size
constexpr short MAX_FILE_CONTENT_SIZE = 512;

string get_file_content(const string& filename)
{
    fstream f;
    string out {};
    out.reserve(MAX_FILE_CONTENT_SIZE);

    f.open(filename.c_str(), fstream::in);

    if (!f.is_open())
    {
        SDL_Log("Failed to open file: %s", filename.c_str());
        return "";
    }

    char c;
    while(f.get(c))
    {
        if (out.size() >= out.capacity())
        {
            SDL_Log("Reallocating file contents string." 
                "If this happens a lot, consider increasing "
                "the reserve size of the file contents string.");
        }

        out.push_back(c);

    }

    return out;
}