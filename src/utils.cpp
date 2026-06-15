#include "utils.hpp"
#include <SDL3/SDL_log.h>
#include <fstream>
#include <sstream>

using std::fstream, std::stringstream;

string get_file_content(string filename)
{
    fstream f;
    f.open(filename.c_str(), fstream::out);

    if (!f.is_open())
    {
        SDL_Log("Failed to open file: %s", filename.c_str());
    }

    stringstream buf;
    buf << f.rdbuf();

    return buf.str();
}