#version 330 core
out vec4 frag_color;
  
in vec3 color;
in vec2 tex_coords;

uniform sampler2D u_texture;

void main()
{
    frag_color = texture(u_texture, tex_coords) * vec4(color, 1.0);
}