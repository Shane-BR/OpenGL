#include "shader.hpp"
#include "utils.hpp"
#include "texture.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <string>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <vector>

using std::string;

// TODO Remove
unsigned int VAO;
std::vector<Shader> shaders;
std::vector<Texture> textures;

class Application
{
private:
    SDL_Window* window;
    string title;
    bool running;

public:
    Application(int w, int h, const string& title);
    void main_loop();
    void render_loop();
    ~Application() { SDL_Quit(); }
};

Application::Application(int w, int h, const string& title)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_Log("Failed to init SDL: %s\n", SDL_GetError());
        throw "Failed to init SDL";
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
        throw "Failed to create window";               
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(window);

    if (!ctx)
    {
        SDL_Log("Failed to create OpenGL context: %s\n", SDL_GetError());
        throw "Failed to create OpenGL context";
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        SDL_Log("Failed to init GLAD.\n");
        throw "Failed to init GLAD";
    }

    glViewport(0, 0, w, h);

    running = true;

    // TODO Remove
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
    glEnableVertexAttribArray(2); // texture coords

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

    // TODO Remove
    shaders[0].use();
    textures[0].bind(0);
    textures[1].bind(1);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
}

int main ()
{
    Application app(800, 600, "OpenGL");

    // TODO remove
    shaders.emplace_back(Shader("../res/shaders/main.vs","../res/shaders/main.fs"));
    textures.emplace_back(Texture("../res/textures/container.jpg"));
    textures.emplace_back(Texture("../res/textures/awesomeface.png"));

    shaders[0].set_int("u_texture1", 0);
    shaders[0].set_int("u_texture2", 1);

    app.main_loop();

    return 0;
}