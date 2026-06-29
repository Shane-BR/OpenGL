#include "input.hpp"
#include <SDL3/SDL_log.h>

void InputManager::addListener(IInputListener* listener)
{
    if (listeners_size >= MAX_INPUT_LISTENERS)
        throw "Too many input listeners! What the hell?\n";

    listeners[listeners_size++] = listener;
}

void InputManager::removeListener(IInputListener* listener)
{
    for (unsigned int i = 0; i < listeners_size; i++)
    {
        IInputListener* temp;
        if (listeners[i] == listener)
        {
            // Swap front and remove
            temp = listeners[listeners_size-1];
            listeners[i] = temp;
            listeners[listeners_size-1] = nullptr;
            listeners_size--;
        }
    }
}

void InputManager::dispatchButtonData(ButtonData data)
{
    for (unsigned int i = 0; i < listeners_size; i++)
    {
        IInputListener* l = listeners[i];
        if (l == nullptr) 
            continue;

        l->onButton(data);
    }

    //SDL_Log("%i, %i", data.id, data.state);

    if (data.state == ButtonState::Pressed)
    {
        addHeld(data.id);
    }
    else if (data.state == ButtonState::Released) 
    {
        removeHeld(data.id);
    }
}

void InputManager::dispatchKeyboardButtonData(SDL_Scancode scancode, bool pressed) 
{
    ButtonID id = getKeyboardButtonID(scancode);
    ButtonState state = pressed ? ButtonState::Pressed : ButtonState::Released;

    dispatchButtonData({id, state});
}

void InputManager::dispatchMouseButtonData(uint8_t mouse_button, bool pressed) 
{
    ButtonID id = getMouseButtonID(mouse_button);
    ButtonState state = pressed ? ButtonState::Pressed : ButtonState::Released;

    dispatchButtonData({id, state});
}

void InputManager::dispatchMouseMotionData(float dx, float dy)
{
    MouseMotionData data = {dx, dy};

    for (unsigned int i = 0; i < listeners_size; i++)
    {
        IInputListener* l = listeners[i];
        if (l == nullptr) 
            continue;

        l->onMouseMotion(data);
    }
}

void InputManager::addHeld(ButtonID button)
{
    held_buttons[held_size] = button;
    // TODO log if overflow
    held_size = (held_size + 1) % MAX_HELD_BUTTONS;
}

void InputManager::removeHeld(ButtonID button)
{
    // O(N), but only small array so no bother.
    // Adding a O(1) lookup structure for held buttons might be overkill.

    for (unsigned int i = 0; i < held_size; i++)
    {
        if (held_buttons[i] == button)
        {
            // Swap front, and lower size (array is unordered)
            ButtonID temp = held_buttons[held_size-1];
            held_buttons[i] = temp;
            held_size--;
        }
    }
}

void InputManager::update()
{

    ButtonState state = ButtonState::HeldDown;

    // Dispatch held buttons
    for (unsigned int i = 0; i < held_size; i++)
    {
        ButtonID id = held_buttons[i];
        
        dispatchButtonData({id, state});
    }
}

// TODO decide whether or not to move this class to its own file
void InputMapper::addBinding(ButtonID button_id, Action action)
{
    bindings.insert({button_id, action});
}

void InputMapper::onButton(ButtonData button_data)
{
    if (bindings.count(button_data.id) == 0)
        return; // No button here

    SDL_Log("Dispatch action: %i", (int)bindings.at(button_data.id));
}

void InputMapper::onMouseMotion(MouseMotionData mouse_data)
{
    SDL_Log("Mouse moved: %f, %f", mouse_data.delta_x, mouse_data.delta_y);
}