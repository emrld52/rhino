#version 330 core

in vec2 uv_coord;
in vec4 worldpos;

out vec4 frag_color;

// currently can only take one light, extremely easily expandable though (merely make an array and pass in multiple light uniforms in render loop)

uniform sampler2D texture_sample1;
uniform vec4 light_pos;
uniform float texture_scale;

void main()
{
   // OLD : frag_color = vec4(col.xyz, 1.0f);
   float dist = clamp(abs(distance(light_pos, worldpos)) * 0.6, 0, 4);
   frag_color = texture(texture_sample1, uv_coord * texture_scale) / dist;
}