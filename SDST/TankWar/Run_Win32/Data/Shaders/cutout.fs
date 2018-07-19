#version 420 core

// Suggest always manually setting bindings - again, consitancy with 
// other rendering APIs and well as you can make assumptions in your
// engine without having to query
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec2 passUV; 
in vec4 passColor; 

out vec4 outColor; 

void main( void )
{
   vec4 tex_color = texture( gTexDiffuse, passUV ); 
   vec4 final_color = tex_color * passColor;

   if (final_color.a <= .5f) {
      discard; 
   }

   outColor = final_color; 
}
