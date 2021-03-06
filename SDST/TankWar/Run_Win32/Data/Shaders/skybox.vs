#version 420 core

// Attributes
in vec3 POSITION;
in vec4 COLOR;

out vec3 passWorldPosition;
out vec4 passColor; 

layout(binding=0, std140) uniform uboCamera 
{
   mat4 VIEW;
   mat4 PROJECTION; 
}; 

uniform mat4 MODEL;


void main( void )
{
   // since I don't want to translate
   vec4 local_pos = vec4( POSITION, 0.0f );  

   vec4 world_pos = MODEL * local_pos;  // assume local is world for now;
   passWorldPosition = world_pos.xyz;  

   vec4 camera_pos = VIEW * world_pos;
   // projection relies on a 1 being present, so add it back
   vec4 clip_pos = PROJECTION * vec4(camera_pos.xyz, 1); 
   // we only render where depth is 1.0 (equal, ie, where have we not drawn)
   // so z needs to be one for all these
   clip_pos.z = clip_pos.w; // z must equal w.  We set the z, not the w, so we don't affect the x and y as well

   passColor = COLOR; 
   gl_Position = clip_pos; // we pass out a clip coordinate
}
