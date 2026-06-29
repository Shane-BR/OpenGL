#include "input.hpp"
#include "shader.hpp"
#include "utils.hpp"
#include "texture.hpp"
#include "camera.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <string>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::string;

// FIXME Remove/move temp global vars
constexpr unsigned short W_WIDTH = 1600;
constexpr unsigned short W_HEIGHT = 1200;
unsigned int VAO;
std::vector<Shader> shaders;
std::vector<Texture> textures;

Camera cam(0.0f, 0.0f, 0.0f);

float delta_time = 0.0f;
float last_frame = 0.0f; // Time from last frame

glm::vec3 cube_positions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};

class Application
{
private:
    SDL_Window* window;
    InputManager manager; // TODO input test
    InputMapper mapper;
    string title;
    bool running;

public:
    Application(int w, int h, const string& title);
    void mainLoop();
    void renderLoop();
    ~Application() { SDL_Quit(); }
};

// TODO there should be an init function that inits objects that register and init textures, shaders, etc
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

    SDL_HideCursor();
    SDL_SetWindowRelativeMouseMode(window, true);

    running = true;

    mapper.addKeyboardBinding(SDL_SCANCODE_W, Action::MoveForward);
    mapper.addKeyboardBinding(SDL_SCANCODE_S, Action::MoveBakcward);
    mapper.addKeyboardBinding(SDL_SCANCODE_A, Action::MoveLeft);
    mapper.addKeyboardBinding(SDL_SCANCODE_D, Action::MoveRight);
    mapper.addKeyboardBinding(SDL_SCANCODE_E, Action::Use);


    manager.addListener(&mapper);

    // TODO Remove
    float vertices[] 
    = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    //unsigned int indices[] =
    //{
    //    0, 1, 3, // first triangle
    //    1, 2, 3 // second triangle
    //};

    unsigned int VBO; 
    //unsigned int EBO;

    /* Basic quad buffer */

    // VAO (Holds VBO, EBO, and attributes)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO
    //glGenBuffers(1, &EBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // position

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1); // texture coords

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);  

}

void Application::mainLoop()
{
    SDL_Event event;

    while (running) 
    {
        
        renderLoop();

        float current_frame = SDL_GetTicks() / 1000.0f;
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        manager.update();

        // TODO Abstract to some other function/method/class
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
                    SDL_Log("Application closed by the user.\n");
                    break;
                // TODO call mouse input handler for following mouse events
                case SDL_EVENT_MOUSE_MOTION:
                    manager.dispatchMouseMotionData(event.motion.xrel, event.motion.yrel);
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    manager.dispatchMouseButtonData(event.button.button, event.button.down);
                    break;
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                    manager.dispatchKeyboardButtonData(event.key.scancode, event.key.down);
                    break; 
                default:
                    //SDL_Log("Unhandled Event.\n");
                    break;
            }
        }
    }
}

void Application::renderLoop()
{
    // Draw
    // TODO this code should, in the future, call a "render queue" that calls all
    // code that renders objects in various classes etc.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO Remove
    shaders[0].use();
    textures[0].bind(0);
    textures[1].bind(1);

    glm::mat4 view = cam.getViewMatrix();

    glm::mat4 projection = glm::perspective(glm::radians(cam.fov), (float)W_WIDTH / W_HEIGHT, 0.1f, 100.0f);

    shaders[0].setMat4("view", view);
    shaders[0].setMat4("projection", projection);

    // Draw a few cubes
    glBindVertexArray(VAO);
    for (unsigned char i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube_positions[i]);
        float angle = (SDL_GetTicks() / 1000.0f) * glm::radians(50.0f + i*10);
        model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.0f));

        shaders[0].setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
}

int main()
{
    Application app(W_WIDTH, W_HEIGHT, "OpenGL");

    // TODO remove
    shaders.emplace_back(Shader("../res/shaders/main.vs","../res/shaders/main.fs"));
    textures.emplace_back(Texture("../res/textures/container.jpg"));
    textures.emplace_back(Texture("../res/textures/awesomeface.png"));

    shaders[0].setInt("u_texture1", 0);
    shaders[0].setInt("u_texture2", 1);

    app.mainLoop();

    return 0;
}