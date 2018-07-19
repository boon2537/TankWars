#version 420 core

layout (binding=7, std140) uniform cFogBlock
{
   vec4 FOG_COLOR; 
   float FOG_NEAR_PLANE;
   float FOG_FAR_PLANE;
   float FOG_NEAR_FACTOR;
   float FOG_FAR_FACTOR;
};

// Uniforms ==============================================
// Constants
uniform float GAME_TIME = 0;

// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec3 passViewPos;
in vec2 passUV; 
in vec4 passColor; 

out vec4 outColor;

vec4 ApplyFog( vec4 color, float view_depth )
{
   #if defined(FOG)
      float fog_factor = smoothstep( FOG_NEAR_PLANE, FOG_FAR_PLANE, view_depth ); 
      fog_factor = FOG_NEAR_FACTOR + (FOG_FAR_FACTOR - FOG_NEAR_FACTOR) * fog_factor; 
      color = mix( color, FOG_COLOR, fog_factor ); 
   #endif

   return color; 
}

void main() 
{
   vec2 uv_offset = vec2(0);
   #if defined(ROLLING)
      uv_offset = passUV + vec2(GAME_TIME * 1.f); 
   #endif

   vec4 diffuse = texture(gTexDiffuse, passUV + uv_offset);
   diffuse *= diffuse.a; // multiply alpha through
   outColor = diffuse * passColor;  

   outColor = ApplyFog(outColor, passViewPos.z);
}