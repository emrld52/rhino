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

// window dimensions

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

// camera stuff for allowing the navigation of 3d space

#define CAMERA_MOV_SPEED 3.0f
#define CAMERA_SENS 0.004f
#define CAMERA_FOV 60.0f

struct camera_transform {
    vec3 posititon;
    vec3 front;
    vec3 up;
    vec3 direction;
    float mov_speed;
    float yaw, pitch;
};

struct camera_transform cam;

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

// mouse-look handling

struct mouse_cursor {
    double x_pos, y_pos;
    float sens;
};

struct mouse_cursor cursor;

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    double x_delta, y_delta;
    
    x_delta = x_pos - cursor.x_pos;
    y_delta = y_pos - cursor.y_pos;

    cam.yaw += x_delta * cursor.sens;
    cam.pitch -= y_delta * cursor.sens;

    // clamp viewing angle

    if(cam.pitch > glm_rad(89.0f)) cam.pitch = glm_rad(89.0f);
    else if(cam.pitch < glm_rad(-89.0f)) cam.pitch = glm_rad(-89.0f);

    cursor.x_pos = x_pos;
    cursor.y_pos = y_pos;

    cam.direction[0] = cos(cam.yaw) * cos(cam.pitch);
    cam.direction[1] = sin(cam.pitch);
    cam.direction[2] = sin(cam.yaw) * cos(cam.pitch);

    glm_normalize(cam.direction);

    glm_vec3_copy(cam.direction, cam.front);
}

// handles movement and input, lots of linear algebra stuff to calculate directions with rotations

void input_handling(GLFWwindow* window) {
    // quick escape
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) cam.mov_speed = CAMERA_MOV_SPEED * 2;
    else cam.mov_speed = CAMERA_MOV_SPEED;

    // side movement

    vec3 cam_cross;
    glm_cross(cam.direction, cam.up, cam_cross);
    glm_normalize(cam_cross);
    
    vec3 to_apply_side;
    glm_vec3_mul((vec3){cam.mov_speed * delta_time, cam.mov_speed * delta_time, cam.mov_speed * delta_time}, cam_cross, to_apply_side);

    // in and out movement

    vec3 to_apply_in_out;
    glm_vec3_mul((vec3){cam.mov_speed * delta_time, cam.mov_speed * delta_time, cam.mov_speed * delta_time}, cam.front, to_apply_in_out);

    vec3 to_apply_up_down;
    glm_vec3_mul((vec3){cam.mov_speed * delta_time, cam.mov_speed * delta_time, cam.mov_speed * delta_time}, cam.up, to_apply_up_down);

    if(glfwGetKey(window, GLFW_KEY_W)) 
        glm_vec3_add(cam.posititon, to_apply_in_out, cam.posititon);
    else if(glfwGetKey(window, GLFW_KEY_S)) 
        glm_vec3_sub(cam.posititon, to_apply_in_out, cam.posititon);
    if(glfwGetKey(window, GLFW_KEY_D)) 
        glm_vec3_add(cam.posititon, to_apply_side, cam.posititon);
    else if(glfwGetKey(window, GLFW_KEY_A)) 
        glm_vec3_sub(cam.posititon, to_apply_side, cam.posititon);
    if(glfwGetKey(window, GLFW_KEY_SPACE)) 
        glm_vec3_add(cam.posititon, to_apply_up_down, cam.posititon);
    else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) 
        glm_vec3_sub(cam.posititon, to_apply_up_down, cam.posititon);

}

// program entry

int main(void) {
    // init opengl, set version and profile (core profile)

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // init opengl window (fullscreen, change glfwGetPrimaryMonitor to NULL if you so need/desire windowed mode)

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rhino Framework", NULL, NULL);

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

    cam.mov_speed = CAMERA_MOV_SPEED;
    cursor.sens = CAMERA_SENS;

    // prepare camera

    cam.posititon[2] = 3.0f;

    glm_vec3((vec4){0, 0, -1, 0}, cam.front);
    glm_vec3((vec4){0, 1, 0, 0}, cam.up);

    // init glad (opengl function pointers)

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("\nfailed to init GLAD, could not load process.");
        return -1;
    }

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

    vec3 light_color;


    // begin render loop, check input and swap buffers


    while(!glfwWindowShouldClose(window)) {
        // read any input, used to exit program if forced
        input_handling(window);

        glUseProgram(shader_program);

        // set blank greenish background and clear screen
        glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set up projection

        glm_mat4_identity(proj);
        glm_perspective(glm_rad(CAMERA_FOV), width/height, 0.1f, 100.0f, proj);
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (float*)proj);

        // camera view

        glm_mat4_identity(view);

        float camX = sin(time * 0.5f) * 10;
        float camZ = cos(time * 0.5f) * 10;

        vec3 target;

        glm_vec3_zero(target);

        glm_vec3_add(cam.posititon, cam.front, target);

        glm_lookat(cam.posititon, target, cam.up, view);

        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

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
        glUniform1i(glGetUniformLocation(shader_program, "texture_sample1"), 0);
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
    }

    // exit program, if havent exited manually

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    glfwTerminate();

    printf("\nexited program successfully");

    return 0;
}