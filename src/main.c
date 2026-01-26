// includes

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "libs/stb_image.h"

#include "libs/cglm/cglm.h"

// rhino headers

#include "shaders.h"
#include "textures.h"
#include "rhino_callbacks.h"
#include "rhino_global.h"

// window dimensions

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

rhino_state rhino;

// time between each frame, used for things such as values that change overtime to stay consistent (e.g movement)

float delta_time = 0.01f;
float time;

float width, height;

#define PRINT_FRAME_TIME_PER_SECONDS 1.0f

// resize gl viewport as window is resized, print debug info also

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    printf("\nwindow resized to %dx%d", width, height);
    width = (float)width;
    height = (float)height;
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    double x_delta, y_delta;
    
    x_delta = x_pos - rhino.mouse.x_pos;
    y_delta = y_pos - rhino.mouse.y_pos;

    rhino.cam.yaw += x_delta * rhino.mouse.sens;
    rhino.cam.pitch -= y_delta * rhino.mouse.sens;

    // clamp viewing angle

    if(rhino.cam.pitch > glm_rad(89.0f)) rhino.cam.pitch = glm_rad(89.0f);
    else if(rhino.cam.pitch < glm_rad(-89.0f)) rhino.cam.pitch = glm_rad(-89.0f);

    rhino.mouse.x_pos = x_pos;
    rhino.mouse.y_pos = y_pos;

    rhino.cam.direction[0] = cos(rhino.cam.yaw) * cos(rhino.cam.pitch);
    rhino.cam.direction[1] = sin(rhino.cam.pitch);
    rhino.cam.direction[2] = sin(rhino.cam.yaw) * cos(rhino.cam.pitch);

    glm_normalize(rhino.cam.direction);

    glm_vec3_copy(rhino.cam.direction, rhino.cam.front);
}

// program entry

int main(void) {
    // init opengl, set version and profile (core profile)

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // init opengl window (fullscreen, change glfwGetPrimaryMonitor to NULL if you so need/desire windowed mode)

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rhino Framework", NULL, NULL);

    rhino.window = window;

    width = WINDOW_WIDTH;
    height = WINDOW_HEIGHT;

    if (window == NULL) {
        printf("\nfailed to create a glfw window.");
        glfwTerminate();
        return -1;
    }

    // set context to the window just created

    glfwMakeContextCurrent(window);

    // callbacks

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // input setup

    rhino.cam.mov_speed = CAMERA_MOV_SPEED;
    rhino.mouse.sens = CAMERA_SENS;

    // prepare camera

    rhino.cam.posititon[2] = 3.0f;

    glm_vec3((vec4){0, 0, -1, 0}, rhino.cam.front);
    glm_vec3((vec4){0, 1, 0, 0}, rhino.cam.up);

    // init glad (opengl function pointers)

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("\nfailed to init GLAD, could not load process.");
        return -1;
    }

    glfwSwapInterval(0);

    // pass dimensions into opengl viewport

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // capture mouse

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  


    // ------------ SHADERS ------------ //

    unsigned int shader_program = link_and_compile_shaders("vertex_shader.glsl", "fragment_shader.glsl");

    glUseProgram(shader_program);

    // --- TEXTURES --- //
    
    load_texture("pebbles.jpg", 0);
    load_texture("container.jpg", 1);


    // ------- CUBE DEFINE, VBO + VAO ------- //

    // cube primitive as vertex coordinates

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // create buffer object of gl_array_buffer type and bind to gpu with id

    unsigned int VBO;

    glGenBuffers(1, &VBO);

    // generate a vao (vertex array object)

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // bind vbo and ebo, allocate memory and copy vertices and indices into gpu memory

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // unbind

    glBindVertexArray(0);

    // frametime and fps counter timer

    float last_frame_draw = 0.01f;
    float fps_timer_counter = PRINT_FRAME_TIME_PER_SECONDS;

    // cglm

    // prepare model, view and projection matrices

    mat4 model, view, proj;

    glm_perspective(glm_rad(CAMERA_FOV), width/height, 0.1f, 100.0f, proj);

    unsigned int model_loc, view_loc, proj_loc;

    model_loc = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

    view_loc = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    proj_loc = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (float*)proj);

    glEnable(GL_DEPTH_TEST);

    unsigned int light_pos_loc = glGetUniformLocation(shader_program, "light_pos");
    unsigned int texture_scale_location = glGetUniformLocation(shader_program, "texture_scale");
    unsigned int texture_sample_loc = glGetUniformLocation(shader_program, "texture_sample1");

    // begin render loop, check input and swap buffers


    while(!glfwWindowShouldClose(window)) {
        glUseProgram(shader_program);

        // set blank greenish background and clear screen
        glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set up projection

        glm_mat4_identity(proj);
        glm_perspective(glm_rad(CAMERA_FOV), width/height, 0.1f, 100.0f, proj);
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (float*)proj);

        // input update callback

        rhino_input_update();

        // camera view

        glm_mat4_identity(view);

        // target direction to look at

        vec3 target;

        glm_vec3_zero(target);
        glm_vec3_add(rhino.cam.posititon, rhino.cam.front, target);
        glm_lookat(rhino.cam.posititon, target, rhino.cam.up, view);

        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

        // rendering callback

        rhino_render_update();

        // TO BE MOVED INTO RENDER UPDATE

        // lights

        vec3 light_pos;
        glm_vec3((vec4){cos(time * 2) - sin(time * 2), (cos(time) * 2) + 0.5f, cos(time * 2) + sin(time * 2), 1}, light_pos);

        glUniform4f(light_pos_loc, light_pos[0], light_pos[1], light_pos[2], 1.0f);

        // draw pebble ground

        glm_mat4_identity(model);
        glm_translate(model, (vec3){0, -10.5f, 0});
        glm_scale(model, (vec3){20, 20, 20});

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

        glUniform1f(texture_scale_location, 8);

        glBindVertexArray(VAO);
        glUniform1i(texture_sample_loc, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // draw rotating crate

        glm_mat4_identity(model);
        glm_translate(model, (vec3){0.0f, 1.0f, 0.0f});
        glm_rotate(model, glm_rad(-60.0f * time), (vec3){0.5f, 1.0f, 0.0f});

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

        glBindVertexArray(VAO);
        glUniform1f(texture_scale_location, 1);
        glUniform1i(glGetUniformLocation(shader_program, "texture_sample1"), 1);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // display

        glfwSwapBuffers(window);
        glfwPollEvents();

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

        last_frame_draw = time;

        rhino.delta_time = delta_time;
    }

    // exit program, if havent exited manually

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    glfwTerminate();

    printf("\nexited program successfully");

    return 0;
}