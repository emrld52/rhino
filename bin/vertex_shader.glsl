#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;

out vec2 uv_coord;
out vec4 worldpos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   vec4 world_pos = model * vec4(aPos, 1.0);
   uv_coord = aUv;
   worldpos = world_pos;
   gl_Position = projection * view * world_pos;
}