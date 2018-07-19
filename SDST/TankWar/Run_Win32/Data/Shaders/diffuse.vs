#version 420 core

layout(binding=0, std140) uniform uboCamera 
{
   mat4 VIEW; 
   mat4 PROJECTION; 
}; 

uniform mat4 MODEL;
uniform vec4 TINT = vec4(1.0, 1.0, 1.0, 1.0);

in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;         
out vec2 passUV; 
out vec4 passColor;

void main() 
{
   vec4 local_pos = vec4( POSITION, 1.0f );  
   vec4 world_pos = MODEL * local_pos; 
   vec4 camera_pos = VIEW * world_pos; 
   vec4 clip_pos = PROJECTION * camera_pos; 
   
   passColor = COLOR * TINT; // pass it on. 
   passUV = UV; 
   gl_Position = clip_pos; 
}