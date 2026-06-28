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

    unsigned int getId();

    // Uniform query and assign methods
    void setBool(const string &name, bool val) const;
    void setInt(const string &name, int val) const;
    void setFloat(const string &name, float val) const;
    void setMat4(const string &name, const mat4 &val) const;
};