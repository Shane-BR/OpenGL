#include "shader.hpp"
#include "utils.hpp"
#include "utils.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <cmath>
#include <string>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>

class Application
{
private:
    SDL_Window* window;
    std::string title;
    bool running;

    // TODO refactor this out
    unsigned int VAO, program_id;
    
public:
    bool create(int w, int h, std::string title);
    void main_loop();
    void render_loop();
    ~Application() { SDL_Quit(); }
};

bool Application::create(int w, int h, std::string title)
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
        // first triangle
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,  // top left 
    };

    unsigned int indices[] =
    {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    const string vertex_shader_source = get_file_content("../res/shaders/main.vert");
    
    const string frag_shader_source = get_file_content("../res/shaders/main.frag");

    // getting the id and discarding the actual object is stoopid, but THIS IS TEMPORARY
    program_id = ShaderProgram(vertex_shader_source, frag_shader_source).get_id();

    unsigned int VBO, EBO;

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

    using std::sin;

    float time = SDL_GetTicks();
    time = time / 1000;
    float green_val = (sin(time / 2.0f) + 0.5f);
    int v_color_loc = glGetUniformLocation(program_id, "color");

    glUseProgram(program_id); // Use shader program from earlier
    glUniform4f(v_color_loc, 0.0f, green_val, 0.0f, 1.0f); // Uniform
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
}

int main ()
{
    Application app;
    app.create(800, 600, "OpenGL");
    app.main_loop();

    return 0;
}