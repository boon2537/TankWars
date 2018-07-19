#version 420 core

layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexBorder;

in vec4 passColor;
in vec2 passBorderUV; 
in vec2 passUV; 

out vec4 outColor; 
  
void main() 
{
   vec4 diffuse = texture( gTexDiffuse, passUV );
   vec4 borderColor = texture( gTexBorder, passBorderUV ); 

   //alpha-blend in the border
   vec3 color = mix( diffuse.xyz, borderColor.xyz, borderColor.w ); 

   outColor = vec4( color, diffuse.w ) * passColor;
}