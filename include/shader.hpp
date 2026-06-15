#pragma once

#include <string>

using std::string;

class Shader
{
private:
    unsigned int id;
public:
    Shader(const string& vertex_source, const string& frag_source);
    void use();

    // Uniform query and assign methods
    void set_bool(const string &name, const bool val) const;
    void set_int(const string &name, const int val) const;
    void set_float(const string &name, const float val) const;


    // TODO remove this when no longer needed in main.cpp
    unsigned int get_id() { return id; }
};