#include "shader.hpp"
#include "utils.hpp"
#include "utils.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <string>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>

// TODO decide to keep or make my own implementation
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using std::string;

class Application
{
private:
    SDL_Window* window;
    string title;
    bool running;

    // TODO refactor this out
    unsigned int VAO, program_id, texture;
    
public:
    bool init(int w, int h, const string& title);
    void main_loop();
    void render_loop();
    ~Application() { SDL_Quit(); }
};
// TODO maybe use a constructor
bool Application::init(int w, int h, const string& title)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_Log("Failed to init SDL: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow
    (
        title.c_str(), w, h, SDL_WINDOW_OPENGL
    );

    if (window == NULL)
    {
        SDL_Log("Failed to create SDL window: %s\n", SDL_GetError());
        return false;                
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(window);

    if (!ctx)
    {
        SDL_Log("Failed to create OpenGL context: %s\n", SDL_GetError());
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        SDL_Log("Failed to init GLAD.\n");
        return false;
    }

    glViewport(0, 0, w, h);

    running = true;

    // TODO Refactor this out
    float vertices[] = 
    {
    // positions          // colors           // texture coords
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] =
    {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    const string vertex_shader_source = get_file_content("../res/shaders/main.vs");
    
    const string frag_shader_source = get_file_content("../res/shaders/main.fs");

    // getting the id and discarding the actual object is stoopid, but THIS IS TEMPORARY
    program_id = Shader(vertex_shader_source, frag_shader_source).get_id();

    unsigned int VBO; 
    unsigned int EBO;

    /* Basic quad buffer */

    // VAO (Holds VBO, EBO, and attributes)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1); // color

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2); // texture

    // Texture
    int width, height, nr_channels;
    unsigned char* data = stbi_load("../res/textures/container.jpg", &width, &height, &nr_channels, 0);

    if (!data)
    {
        SDL_Log("Failed to load texture!\n");
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D
    (
        GL_TEXTURE_2D, 0, GL_RGB, width, height,
        0, GL_RGB, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

void Application::main_loop()
{
    SDL_Event event;

    while (running) 
    {
        
        render_loop();

        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
                    SDL_Log("Application closed by the user.\n");
                    break;
                default:
                    //SDL_Log("Unhandled Event.\n");
                    break;
            }
        }
    }
}

void Application::render_loop()
{
    // Draw
    // TODO this code should, in the future, call a "render queue" that calls all
    // code that renders objects in various classes etc.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program_id); // Use shader program from earlier

    glBindTexture(GL_TEXTURE_2D, texture); // Automatically applies to VAO frag uniform
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
}

int main ()
{
    Application app;
    app.init(800, 600, "OpenGL");
    app.main_loop();

    return 0;
}