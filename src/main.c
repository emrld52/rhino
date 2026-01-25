#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include "libs/cglm/cglm.h"

// rhino headers

#include "shaders.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

#define PRINT_FRAME_TIME_PER_SECONDS 1.0f

#define CAMERA_MOV_SPEED 0.5f
#define CAMERA_ZOOM_SPEED 1.0f
#define CAMERA_ROT_SPEED 1.0f

// time between each frame, used for things such as values that change overtime to stay consistent (e.g movement)

float delta_time = 0.01f;
float time;

// resize gl viewport as window is resized, print debug info also

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    printf("\nwindow resized to %dx%d", width, height);
}

// --- INPUT HANDLING --- //

void input_handling(GLFWwindow* window) {
    // quick escape
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
}

int main(void) {
    // init opengl, set version and profile (core profile)

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // init opengl window (fullscreen, change glfwGetPrimaryMonitor to NULL if you so need/desire windowed mode)

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rhino Framework", NULL, NULL);

    if (window == NULL) {
        printf("\nfailed to create a glfw window.");
        glfwTerminate();
        return -1;
    }

    // set context to the window just created

    glfwMakeContextCurrent(window);

    // frame resizing

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // init glad (opengl function pointers)

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("\nfailed to init GLAD, could not load process.");
        return -1;
    }

    // pass dimensions into opengl viewport

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // print info about max number of vertex attribs

    int num_attributes;

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &num_attributes);

    printf("maximum number of vertex attribs : %d", num_attributes);


    // ------------ SHADERS ------------ //

    unsigned int shader_program = link_and_compile_shaders("D:\\!programming\\C\\rhino\\src\\shaders\\vertex_shader.glsl", "D:\\!programming\\C\\rhino\\src\\shaders\\fragment_shader.glsl");

    glUseProgram(shader_program);


    // ------- QUAD DEFINE, VBO + VAO ------- //


    // quad primitive as vertex coordinates

    float vertices[] = {
       -0.5f,  -0.5f,   0.0f, /* bottom left */     0.0f, 0.0f,
        0.5f,  -0.5f,   0.0f, /* bottom right */    1.0f, 0.0f,
       -0.5f,   0.5f,   0.0f, /* top left */        0.0f, 1.0f,
        0.5f,   0.5f,   0.0f, /* top right */       1.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2, // tri 1
        3, 2, 1  // tri 2
    };

    // create buffer object of gl_array_buffer type and bind to gpu with id

    unsigned int VBO, EBO;

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // generate a vao (vertex array object)

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // bind vbo and ebo, allocate memory and copy vertices and indices into gpu memory

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // unbind

    glBindVertexArray(0);

    unsigned int VBO2, EBO2, VAO2;

    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);


    // --- TEXTURES --- //


    // generate texture object in gpu memory and id

    unsigned int pebbles_texture;
    glGenTextures(1, &pebbles_texture);
    glBindTexture(GL_TEXTURE_2D, pebbles_texture);

    // textures, set textures to perform a mirrored repeat when exceeding past 0 - 1 tex coords on x axis

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image with stb image

    int width, height, channels;
    unsigned char* tex_data = stbi_load("assets/img/pebbles.jpg", &width, &height, &channels, 0);

    // quick error check, if image couldnt load then exit the applicaiton

    if(tex_data) {
        // pass texture into gpu memory and generate mipmap

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // image has been loaded into gpu and mipmaps have been generated, no longer needs to be stored in main ram, free the image from ram

        stbi_image_free(tex_data);
    }
    else {
        printf("failed to load texture, exiting program");
        return -1;
    }

    unsigned int crate_texture;

    glGenTextures(1, &crate_texture);
    glBindTexture(GL_TEXTURE_2D, crate_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* crt_dat = stbi_load("assets\\img\\container.jpg", &width, &height, &channels, 0);

    if(crt_dat) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, crt_dat);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(crt_dat);
    }
    else {
        printf("error loading crate texture, exiting");
        return -1;
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pebbles_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, crate_texture);

    // frametime and fps counter timer

    float last_frame_draw = 0.01f;
    float fps_timer_counter = PRINT_FRAME_TIME_PER_SECONDS;

    // CGLM TEST

    unsigned int transformation_matrix_loc = glGetUniformLocation(shader_program, "transform");


    // begin render loop, check input and swap buffers


    while(!glfwWindowShouldClose(window)) {
        // read any input, used to exit program if forced
        input_handling(window);

        // set blank greenish background and clear screen
        glClearColor(0.65f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // get time for shaders and also frametime counter

        time = glfwGetTime();

        // frame time counters, print frametime counter every N seconds as specified in define at top

        delta_time = time - last_frame_draw;

        fps_timer_counter -= delta_time;

        if(fps_timer_counter <= 0) {
            // reset timer
            fps_timer_counter = PRINT_FRAME_TIME_PER_SECONDS;
            printf("\nfps : %f - frametime : %f\n", 1.0f / delta_time, delta_time);
        }

        glUseProgram(shader_program);

        // rebuild matrix every frame and apply position, rotation, and scale

        mat4 transform_matrix;

        glm_mat4_identity(transform_matrix);
        glm_rotate(transform_matrix, glm_rad(180.0f * time), (vec3){0, 0, 1});
        glm_translate(transform_matrix, (vec3){0.5f, -0.5f, 0.0f});
        glm_scale(transform_matrix, (vec3){0.5f, 0.5f, 1.0f});

        glUniformMatrix4fv(transformation_matrix_loc, 1, GL_FALSE, (float *)transform_matrix);

        // draw quad

        glBindVertexArray(VAO);
        glUniform1i(glGetUniformLocation(shader_program, "texture_sample1"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glm_mat4_identity(transform_matrix);
        glm_translate(transform_matrix, (vec3){0.5f, -0.5f, 0.0f});
        glm_rotate(transform_matrix, glm_rad(180.0f * time), (vec3){0, 0, 1});
        glm_scale(transform_matrix, (vec3){0.5f, 0.5f, 1.0f});

        glUniformMatrix4fv(transformation_matrix_loc, 1, GL_FALSE, (float *)transform_matrix);

        glBindVertexArray(VAO2);
        glUniform1i(glGetUniformLocation(shader_program, "texture_sample1"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // display

        glfwSwapBuffers(window);
        glfwPollEvents();

        last_frame_draw = time;
    }

    // exit program, if havent exited manually

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);
    glDeleteTextures(1, &pebbles_texture);

    glfwTerminate();

    printf("\nexited program successfully");

    return 0;
}