#version 330 core

in vec2 uv_coord;
in vec4 worldpos;

out vec4 frag_color;

uniform sampler2D texture_sample1;
uniform vec4 playerpos;

void main()
{
   // OLD : frag_color = vec4(col.xyz, 1.0f);
   float dist = distance(playerpos, worldpos);
   frag_color = texture(texture_sample1, uv_coord) / (log(dist * dist) - 1.5);
}