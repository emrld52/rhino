#include "textures.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

void load_texture(char* texture_path, int texture_unit) {
    // generate texture object

    unsigned int texture;
    glGenTextures(1, &texture);

    // apply to texture unit. only 4 for now, can easily do more later

    switch(texture_unit) {
        case 0:
            glActiveTexture(GL_TEXTURE0);
            break;
        case 1:
            glActiveTexture(GL_TEXTURE1);
            break;
        case 2:
            glActiveTexture(GL_TEXTURE2);
            break;
        case 3:
            glActiveTexture(GL_TEXTURE3);
            break;
    }

    // bind to texture unit and to gl_texture_2d
    
    glBindTexture(GL_TEXTURE_2D, texture);

    // set parameters, repeat texture on wrap, use linear filtering scaling up and nearest scaling down

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // set up width, height and channel variables to copy image info into to pass into gl later. store image as unsigned char data

    int width, height, channels;
    unsigned char* tex_data = stbi_load(texture_path, &width, &height, &channels, 0);

    // error check

    if(tex_data) {
        // pass over loaded image data to gpu and generate mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // free memory used by image
        stbi_image_free(tex_data);
    }
    else {
        printf("error loading texture %s", texture_path);
    }
}