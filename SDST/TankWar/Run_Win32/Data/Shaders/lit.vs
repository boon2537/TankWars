#version 420 core

layout(binding=0, std140) uniform uboCamera 
{
   mat4 VIEW; 
   mat4 PROJECTION; 
}; 

// Uniforms ==============================================
uniform mat4 MODEL;
uniform vec4 TINT = vec4(1.0, 1.0, 1.0, 1.0);

// Attributes ============================================
in vec3 POSITION;
in vec3 NORMAL; 
in vec4 TANGENT; 

in vec4 COLOR;
in vec2 UV; 

// Outputs
out vec3 passViewPos;
out vec3 passWorldPos;
out vec3 passWorldTangent; 
out vec3 passWorldNormal;
out vec3 passWorldBitangent;

out vec2 passUV; 
out vec4 passColor; 

// Entry Point ===========================================
void main( void )
{
   vec4 local_pos = vec4( POSITION, 1.0f );  

   vec4 world_pos = MODEL * local_pos; 
   vec4 camera_pos = VIEW * world_pos; 
   passViewPos = camera_pos.xyz;
   
   vec4 clip_pos = PROJECTION * camera_pos; 

   passUV = UV; 
   passColor = COLOR * TINT; 
   passWorldPos = world_pos.xyz;  

   // get information for tbn space
   passWorldNormal = normalize((MODEL * vec4(NORMAL, 0.0f)).xyz); 
   passWorldTangent = normalize((MODEL * vec4(TANGENT.xyz, 0.0f)).xyz); 
   passWorldBitangent = normalize( cross( passWorldTangent, passWorldNormal ) * TANGENT.w ); 

   gl_Position = clip_pos; // we pass out a clip coordinate
}