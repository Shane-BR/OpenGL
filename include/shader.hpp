#pragma once

#include <glm/glm.hpp>
#include <string>

using std::string, glm::mat4;
class Shader
{
private:
    unsigned int id;
public:
    Shader(const string& vertex_filename, const string& frag_filename);
    void use() const;

    unsigned int get_id();

    // Uniform query and assign methods
    void set_bool(const string &name, const bool val) const;
    void set_int(const string &name, const int val) const;
    void set_float(const string &name, const float val) const;
    void set_mat4(const string &name, const mat4 &val) const;
};