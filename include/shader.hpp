#pragma once

#include <string>

using std::string;

class ShaderProgram
{
private:
    unsigned int id;
public:
    ShaderProgram(const string vertex_source, const string frag_source);
    void use();
    unsigned int get_id() { return id; }
};