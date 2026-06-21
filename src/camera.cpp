#include "camera.hpp"
#include <SDL3/SDL_log.h>
#include <glm/ext/matrix_transform.hpp>

using glm::radians, glm::normalize, glm::cross, glm::lookAt;

Camera::Camera(const vec3& _position, const float _pitch, const float _yaw) 
:   position(_position),
    direction_up(WORLD_UP),
    pitch(_pitch),
    yaw(_yaw)
{
    update_vectors();
}

Camera::Camera(const float x, const float y, const float z, 
    const float _pitch, const float _yaw)
:   Camera(vec3(x, y, z), _pitch, _yaw)
{}

void Camera::update_vectors()
{
    vec3 dir
    (
        cos(radians(yaw)) * cos(::radians(pitch)),
        sin(radians(pitch)),
        sin(radians(yaw)) * cos(radians(pitch))
    );

    direction_front = normalize(dir);

    // Right and up vectors
    direction_right = normalize(cross(direction_front, WORLD_UP));
    direction_up = normalize(cross(direction_right, direction_front));
}

mat4 Camera::get_view_matrix() const
{
    return lookAt(position, position + direction_front, direction_up);
}

void Camera::handle_look_around
(
    const float x_offset, const float y_offset, 
    const bool constrain_pitch, const float pitch_max, const float pitch_min
)
{
    pitch -= y_offset * (look_sensitivity);
    yaw += x_offset * (look_sensitivity);

    if (constrain_pitch)
    {
        if (pitch > pitch_max)
        {
            pitch = pitch_max;
        }
        else if (pitch < pitch_min)
        {
            pitch = pitch_min;
        }
    }

    update_vectors();
}

void Camera::handle_movement(const CameraDirection direction, const float delta_time)
{
    float velocity = movement_speed * delta_time;
    switch (direction) 
    {
        case CameraDirection::FORWARD:
            position += velocity * direction_front;
            break;
        case CameraDirection::BACKWARD:
            position -= velocity * direction_front;
            break;
        case CameraDirection::LEFT:
            position -= velocity * direction_right;
            break;
        case CameraDirection::RIGHT:
            position += velocity * direction_right;
            break;
    }

    update_vectors();
}

void Camera::handle_zoom(const float zoom, const float delta_time)
{
    fov += zoom * delta_time;
}

void Camera::increase_movement_speed(const float factor)
{
    movement_speed *= factor;
}