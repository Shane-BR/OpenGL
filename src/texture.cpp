#include "texture.hpp"
#include <SDL3/SDL_log.h>
#include <glad/glad.h>

// TODO decide to keep or make my own implementation
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const string& filename)
{

    stbi_set_flip_vertically_on_load(true);

    int nr_channels;
    unsigned char* data = 
        stbi_load
        (
            filename.c_str(), 
            &width, &height, &nr_channels, 0
        );

    if (!data)
    {
        SDL_Log("Failed to load texture at: %s \n", filename.c_str());
        throw "Failed to load texture";
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // TODO decide to set texture params or not.

    // Figure out what color channels the texture contains
    // FIXME find a way to tell the texture about greyscale and alpha
    int format = 0;
    switch (nr_channels) 
    {
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            SDL_Log("Unexpected channel number when loading texture: %i", nr_channels);
            throw "Unexpected texture channel number";
            break;
    }

    glTexImage2D
    (
        GL_TEXTURE_2D, 0, GL_RGB, width, height,
        0, format, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

unsigned int Texture::get_id()
{
    return id;
}