#pragma once

#include <string>

using std::string;

class ShaderProgram
{
private:
    unsigned int id;
public:
    ShaderProgram(string vertex_source, string frag_source);
    void use();
    unsigned int get_id() { return id; }
};