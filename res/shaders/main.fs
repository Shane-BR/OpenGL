#version 330 core
out vec4 frag_color;
  
in vec2 tex_coords;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

void main()
{
    frag_color = mix(texture(u_texture1, tex_coords), texture(u_texture2, tex_coords), 0.2);
}