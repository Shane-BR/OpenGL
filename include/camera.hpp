#pragma once

#include <glm/glm.hpp>

using glm::vec3, glm::mat4;

constexpr vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

enum class CameraDirection
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

/* TODO
    Likely the camera itself shouldn't handle movement but instead be 'attached' to an
    object that moves and the camera updates its vectors from that.
*/

class Camera 
{
private:
    // Relative position and direction vectors
    vec3 position;
    vec3 direction_up;
    vec3 direction_right;
    vec3 direction_front;

    // Rotation
    float pitch, yaw;

    float look_sensitivity = 0.1f; // TODO default value should be pulled from user settings
    float movement_speed = 2.5f;

    void update_vectors();
public:
    float fov = 60.0f; // TODO default value should be pulled from user settings

    Camera(const vec3& _position, const float _pitch=0.0f, const float _yaw=-90.0f);
    Camera(const float x, const float y, const float z, 
        const float _pitch=0.0f, const float _yaw=-90.0f);
    mat4 get_view_matrix() const;
    // pitch_max/_min is only used when constrain_pitch is true
    void handle_look_around
    (
        const float x_offset, const float y_offset, 
        const bool constrain_pitch = true, const float pitch_max = 89.0f, const float pitch_min = -89.0f
    );
    void handle_movement(const CameraDirection direction, const float delta_time);
    void handle_zoom(const float zoom, const float delta_time);
    void increase_movement_speed(const float factor);
};