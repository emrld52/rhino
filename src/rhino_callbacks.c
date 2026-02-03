// libs

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// rhino headers

#include "shaders.h"
#include "textures.h"
#include "rhino_callbacks.h"
#include "rhino_global.h"

// init variables here

void rhino_start() {

}

// camera movement and anything tied to user input goes here

void rhino_input_update() {
    // quick escape
    if(glfwGetKey(rhino.window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(rhino.window, true);

    if(glfwGetKey(rhino.window, GLFW_KEY_LEFT_SHIFT)) rhino.cam.mov_speed = CAMERA_MOV_SPEED * 2;
    else rhino.cam.mov_speed = CAMERA_MOV_SPEED;

    // side movement

    vec3 delta_time_vec;
    glm_vec3((vec4){rhino.cam.mov_speed * rhino.delta_time, rhino.cam.mov_speed * rhino.delta_time, rhino.cam.mov_speed * rhino.delta_time, 1.0f}, delta_time_vec);

    vec3 cam_cross;
    glm_cross(rhino.cam.direction, rhino.cam.up, cam_cross);
    glm_normalize(cam_cross);
    
    vec3 to_apply_side;
    glm_vec3_mul(delta_time_vec, cam_cross, to_apply_side);

    // in and out movement

    vec3 to_apply_in_out;
    glm_vec3_mul(delta_time_vec, rhino.cam.front, to_apply_in_out);

    vec3 to_apply_up_down;
    glm_vec3_mul(delta_time_vec, rhino.cam.up, to_apply_up_down);

    if(glfwGetKey(rhino.window, GLFW_KEY_W)) 
        glm_vec3_add(rhino.cam.posititon, to_apply_in_out, rhino.cam.posititon);
    else if(glfwGetKey(rhino.window, GLFW_KEY_S)) 
        glm_vec3_sub(rhino.cam.posititon, to_apply_in_out, rhino.cam.posititon);

    if(glfwGetKey(rhino.window, GLFW_KEY_D)) 
        glm_vec3_add(rhino.cam.posititon, to_apply_side, rhino.cam.posititon);
    else if(glfwGetKey(rhino.window, GLFW_KEY_A)) 
        glm_vec3_sub(rhino.cam.posititon, to_apply_side, rhino.cam.posititon);

    if(glfwGetKey(rhino.window, GLFW_KEY_SPACE)) 
        glm_vec3_add(rhino.cam.posititon, to_apply_up_down, rhino.cam.posititon);
    else if(glfwGetKey(rhino.window, GLFW_KEY_LEFT_CONTROL)) 
        glm_vec3_sub(rhino.cam.posititon, to_apply_up_down, rhino.cam.posititon);

    // unlock or lock mouse

    if(glfwGetKey(rhino.window, GLFW_KEY_U) == GLFW_PRESS) {
        rhino.mouse.unlocked = !rhino.mouse.unlocked;

        glfwWaitEventsTimeout(0.5);

        if(rhino.mouse.unlocked) glfwSetInputMode(rhino.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  
        else glfwSetInputMode(rhino.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    }
}

// render objects here

void rhino_render_update()
{
    
}

// cleanup anything in memory here

void rhino_exit() {

}