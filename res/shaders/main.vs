#version 330 core
layout (location = 0) in vec3 a_pos; // the position variable has attribute position 0
layout (location = 1) in vec3 a_color;

out vec3 color;

void main()
{
    gl_Position = vec4(a_pos, 1.0); // see how we directly give a vec3 to vec4's constructor
    color = a_color;
}