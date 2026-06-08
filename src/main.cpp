#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
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
    int window_width, window_height;
    std::string title;
    bool running;
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
                    SDL_Log("Unhandled Event.\n");
                    break;
            }
        }
    }
}

void Application::render_loop()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(window);
}

int main ()
{
    Application app;
    app.create(800, 600, "OpenGL");
    app.main_loop();

    return 0;
}