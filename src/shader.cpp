#include "shader.hpp"
#include "utils.hpp"
#include <SDL3/SDL_log.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const string& vertex_filename, const string& frag_filename)
{
    const string vertex_source = get_file_content(vertex_filename);
    const string frag_source = get_file_content(frag_filename);

    if (vertex_source.empty())
    {
        SDL_Log("Failed to read vertex shader at '%s'\n", vertex_filename.c_str());
        throw "Failed to read shader file";
    }

    if (frag_source.empty())
    {
        SDL_Log("Failed to read fragment shader at '%s'\n", frag_filename.c_str());
        throw "Failed to read shader file";
    }

    // Vertex Shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* temp = vertex_source.c_str();
    glShaderSource(vertex_shader, 1, &temp, NULL);
    glCompileShader(vertex_shader);

    int  success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        SDL_Log("ERROR: Vertex Shader Compilation Failed: \n%s", info_log);
        throw "Shader Compilation Failure";
    }

    // Fragment Shader
    unsigned int frag_shader;
    temp = frag_source.c_str();
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &temp, NULL);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
        SDL_Log("ERROR: Fragment Shader Compilation Failed: \n%s", info_log);
        throw "Shader Compilation Failure";
    }

    // Shader program
    id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, frag_shader);
    glLinkProgram(id);

    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, 512, NULL, info_log);
        SDL_Log("ERROR: Failed To Link Shader Program: \n%s", info_log);
        throw "Shader Link Failure";
    }
}

void Shader::use() const
{
    glUseProgram(id);
}

unsigned int Shader::get_id()
{
    return id;
}

void Shader::set_bool(const string &name, const bool val) const
{
    use();
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)val);
}

void Shader::set_int(const string &name, const int val) const
{
    use();
    glUniform1i(glGetUniformLocation(id, name.c_str()), val);
}

void Shader::set_float(const string &name, const float val) const
{
    use();
    glUniform1f(glGetUniformLocation(id, name.c_str()), val);
}

void Shader::set_mat4(const string &name, const mat4 &val) const
{
    use();
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}