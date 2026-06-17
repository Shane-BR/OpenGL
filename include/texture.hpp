#pragma once

#include <string>

using std::string;

class Texture
{
private:
    unsigned int id;
    int width, height;

public:
    Texture(const string& filename);
    // Binds the texture with a specified texture unit.
    void bind(int unit);
    unsigned int get_id();
};