#pragma once

#include <SDL3/SDL_scancode.h>
#include <cstdint>
#include <unordered_map>

using std::unordered_map;

using ButtonID = unsigned int;

/* 
    Keyboard and mouse buttons may share the same numerical value in SDL, so we 
    use these silly little const expression functions to convert.
*/

constexpr ButtonID getKeyboardButtonID(SDL_Scancode scancode)
{
    return (ButtonID)scancode;
}

constexpr ButtonID getMouseButtonID(uint8_t mouse_button)
{
    return mouse_button + SDL_SCANCODE_COUNT;
}

enum ButtonState { Pressed, Released, HeldDown };

struct ButtonData 
{
    ButtonID id;
    ButtonState state;
};

struct MouseMotionData 
{
    float delta_x;
    float delta_y;
};

// Interface
class IInputListener
{
public:
    virtual ~IInputListener() = default;
    virtual void onButton(ButtonData button_state) = 0;
    virtual void onMouseMotion(MouseMotionData mouse_state) = 0;
};

class InputManager 
{
private:
    // If we need more than 16 input listeners then we are doing something wrong
    // NO DYNAMIC ALLOCATION FOR YOU!!!
    static constexpr unsigned int MAX_INPUT_LISTENERS = 16;
    static constexpr unsigned int MAX_HELD_BUTTONS = 8;
    unsigned int listeners_size = 0;
    
    IInputListener* listeners[MAX_INPUT_LISTENERS];

    // Can't hold more than 8 buttons at the same time
    ButtonID held_buttons[MAX_HELD_BUTTONS];
    unsigned int held_size = 0;

    void addHeld(ButtonID button);
    void removeHeld(ButtonID button);
    void dispatchButtonData(ButtonData data);
public:
    void addListener(IInputListener* listener);
    void removeListener(IInputListener* listener);
    void dispatchKeyboardButtonData(SDL_Scancode scancode, bool pressed);
    void dispatchMouseButtonData(uint8_t mouse_button, bool pressed);
    void dispatchMouseMotionData(float dx, float dy);
    void update();
};

// TODO this is placeholder to test input mapper
enum class Action
{
    MoveForward,
    MoveBakcward,
    MoveLeft,
    MoveRight,
    Attack,
    Use
};

class InputMapper : public IInputListener
{
private:
    unordered_map<ButtonID, Action> bindings;
    void addBinding(ButtonID button_id, Action action);
public:
    void addMouseBinding(uint8_t mouse_button, Action action) 
        { addBinding(getMouseButtonID(mouse_button), action); }

    void addKeyboardBinding(SDL_Scancode scancode, Action action) 
        { addBinding(getKeyboardButtonID(scancode), action); }

    void onButton(ButtonData button_data) override;
    void onMouseMotion(MouseMotionData mouse_data) override;
};