#version 420 core

uniform mat4 PROJECTION; 

in vec3 POSITION;
in vec4 COLOR;       // NEW - GLSL will use a Vector4 for this; 
in vec2 UV;         

out vec2 passUV; 
out vec4 passColor;  // NEW - to use it in the pixel stage, we must pass it.
out vec2 passBorderUV;

vec2 BORDER_UVS[] = { vec2(0.0), vec2(0.0f, 1.0f), vec2(1.0f, 0.0f), vec2(1.0f) };

void main() 
{
   vec4 local_pos = vec4( POSITION, 1 ); 
   vec4 clip_pos = PROJECTION * local_pos; 

   passColor = COLOR; // pass it on. 
   passBorderUV = BORDER_UVS[gl_VertexID % 4]; 
   passUV = UV; 
   gl_Position = clip_pos; 
}