#pragma once

#include <string>

using std::string;

class Shader
{
private:
    unsigned int id;
public:
    Shader(const string& vertex_filename, const string& frag_filename);
    void use() const;

    // Uniform query and assign methods
    void set_bool(const string &name, const bool val) const;
    void set_int(const string &name, const int val) const;
    void set_float(const string &name, const float val) const;
};