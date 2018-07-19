#version 420 core

#define MAX_LIGHTS 8

//Structs
struct light_t 
{
   vec4 color;

   vec3 position; 
   float uses_shadow; 

   vec3 direction; 
   float direction_factor;

   vec3 attenuation; 
   float dot_inner_angle; 

   vec3 spec_attenuation; 
   float dot_outer_angle; 

   mat4 shadow_vp;
}; 

//--------------------------------------------------------------------------------------
struct light_factor_t 
{
   vec3 diffuse; 
   vec3 specular; 
}; 

//UBO====================================================
layout (binding=5, std140) uniform cLightBlock
{
   vec4 AMBIENT; // xyz color, w intensity
   light_t LIGHTS[MAX_LIGHTS];
};

//--------------------------------------------------------------------------------------
layout (binding=6, std140) uniform cSpecBlock
{
   float SPECULAR_AMOUNT; // shininess (0 to 1)
   float SPECULAR_POWER; // smoothness (1 to whatever)
   vec2 padding0;  
}; 

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
uniform vec3 EYE_POSITION;  // camera related
uniform vec4 RENDER_MIX = vec4(1, 1, 1, 0);

// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal; 
layout(binding = 8) uniform sampler2DShadow gTexShadow;

// Attributes ============================================
in vec3 passViewPos;
in vec3 passWorldPos;
in vec3 passWorldNormal;
in vec3 passWorldTangent; 
in vec3 passWorldBitangent; 

in vec2 passUV; 
in vec4 passColor; 

out vec4 outColor; 

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
   if (inStart == inEnd)
   {
      return (outStart + outEnd) * 0.5f;
   }

   float inRange = inEnd - inStart;
   float outRange = outEnd - outStart;
   float inFromStart = inValue - inStart;
   float fractionIntoRange = inFromStart / inRange;
   float outRelativeToStart = fractionIntoRange * outRange;
   return outRelativeToStart + outStart;
}

vec4 ApplyFog( vec4 color, float view_depth )
{
   #if defined(FOG)
      float fog_factor = smoothstep( FOG_NEAR_PLANE, FOG_FAR_PLANE, view_depth ); 
      fog_factor = FOG_NEAR_FACTOR + (FOG_FAR_FACTOR - FOG_NEAR_FACTOR) * fog_factor; 
      color = mix( color, FOG_COLOR, fog_factor ); 
   #endif

   return color; 
}

// i: light intensity
// d: distance from light
// a: attenuation factors;
float GetAttenuation(float i, float d, vec3 a)
{
   return i / (a.x + (d * a.y) + (d * d * a.z));
}

// return 1 if fully lit, 0 if should be fully in shadow (ignores light)
float GetShadowFactor( vec3 position, vec3 normal, light_t light )
{
   float shadow = light.uses_shadow;
   if (shadow == 0.0f) {
      return 1.f; 
   }
   
   // so, we're lit, so we will use the shadow sampler
   float bias_factor = max( dot( light.direction, normal ), 0.0f ); 
   bias_factor = sqrt(1 - (bias_factor * bias_factor)); 
   position -= light.direction * bias_factor * .25f; 

   vec4 clip_pos = light.shadow_vp * vec4(position, 1.0f);
   vec3 ndc_pos = clip_pos.xyz / clip_pos.w; 

   // put from -1 to 1 range to 0 to 1 range
   ndc_pos = (ndc_pos + vec3(1)) * .5f;
   
   // can give this a "little" bias
   // treat every surface as "slightly" closer"
   // returns how many times I'm pass (GL_LESSEQUAL)
   float is_lit = texture( gTexShadow, ndc_pos ).r; 
   // float my_depth = ndc_pos.z; 
   
   // use this to feathre shadows near the border
   float min_uv = min( ndc_pos.x, ndc_pos.y ); 
   float max_uv = max( ndc_pos.x, ndc_pos.y ); 
   float blend = 1.0f - min( smoothstep(0.0f, .05f, min_uv), smoothstep(1.0, .95, max_uv) ); 

   // step returns 0 if nearest_depth is less than my_depth, 1 otherwise.
   // if (nearest_depth) is less (closer) than my depth, that is shadow, so 0 -> shaded, 1 implies light
   // float is_lit = step( my_depth, nearest_depth ); // 

   // scale by shadow amount
   return mix( light.uses_shadow * is_lit, 1.0f, blend );  
}

light_factor_t CalculateLightFactor(vec3 position, 
   vec3 eye_dir, 
   vec3 normal, 
   light_t light, 
   float spec_factor, 
   float spec_power)
{
   light_factor_t lf; 

   vec3 light_color = light.color.xyz;

   // get my direction to the light, and distance
   vec3 light_dir = light.position - position; // direction TO the light
   float dist = length(light_dir); 
   light_dir /= dist; 

   float shadowing = GetShadowFactor( position, normal, light ); 

   vec3 light_forward = normalize(light.direction); 

   // get the power
   float light_power = light.color.w; 

   // figure out how far away angle-wise I am from the forward of the light (useful for spot lights)
   float dot_angle = dot( light_forward, -light_dir ); 
   float angle_attenuation = smoothstep( light.dot_outer_angle, light.dot_inner_angle, dot_angle ); 

   // falloff for spotlights.
   //float angle_attenuation = RangeMap(dot_angle, light.dot_outer_angle, light.dot_inner_angle, 0, 1);
   //angle_attenuation = smoothstep(1, 0, angle_attenuation);

   light_power = light_power * angle_attenuation; 

   // get actual direction light is pointing (spotlights point in their "forward", point lights point everywhere (ie, toward the point))
   light_dir = mix(light_dir, -light_forward, light.direction_factor);

   float attenuation = clamp( GetAttenuation( light_power, dist, light.attenuation ), 0, 1 ); 
   float spec_attenuation = clamp( GetAttenuation( light_power, dist, light.spec_attenuation ), 0, 1 );

   // Finally, calculate dot3 lighting
   float dot3 = dot( light_dir, normal ); 
   float diffuse_factor = clamp( attenuation * dot3, 0.0f, 1.0f );

   // specular
   vec3 r = reflect(-light_dir, normal); 
   float spec_amount = max(dot(r, eye_dir), 0.0f); 
   float spec_intensity = (spec_attenuation * spec_factor) * pow(spec_amount, spec_power);

   lf.diffuse = shadowing * light_color * diffuse_factor;
   lf.specular = shadowing * light_color * spec_intensity; 

   return lf; 
}

light_factor_t CalculateLighting(vec3 world_pos, 
   vec3 eye_dir, 
   vec3 normal, 
   float spec_factor, 
   float spec_power) 
{
   light_factor_t lf; 
   lf.diffuse = (AMBIENT.xyz * passColor.xyz) * AMBIENT.w; 
   lf.specular = vec3(0.0f); 

   spec_factor *= SPECULAR_AMOUNT; 
   spec_power *= SPECULAR_POWER; 

   for (uint i = 0; i < MAX_LIGHTS; ++i) {
      light_factor_t l = CalculateLightFactor( world_pos, eye_dir, normal, LIGHTS[i], spec_factor, spec_power ); 
      lf.diffuse += l.diffuse;
      lf.specular += l.specular; 
   }

   lf.diffuse = clamp( lf.diffuse, vec3(0.0f), vec3(1.0f) ); 
   return lf; 
}

vec3 WorldNormalToColor(vec3 normal)
{
   return (normal + vec3(1.0f)) * .5f; 
}

// Entry Point ===========================================
void main( void )
{
   vec2 uv_offset = vec2(0);
   #if defined(ROLLING)
      uv_offset = passUV + vec2(GAME_TIME * 1.f); 
   #endif

   // Get the surface colour
   vec4 tex_color = texture( gTexDiffuse, passUV + uv_offset ); 
   vec3 normal_color = texture( gTexNormal, passUV + uv_offset ).xyz;

   // Interpolation is linear, so normals become not normal
   // over a surface, so renormalize it. 
   vec3 world_vnormal = normalize(passWorldNormal);

   // Get the surface to world matrix
   vec3 world_vtan = normalize(passWorldTangent); 
   vec3 world_vbitan = normalize(passWorldBitangent); 
   mat3 surface_to_world = mat3(world_vtan, world_vbitan, world_vnormal); 

   // range map it to a surface normal
   vec3 surface_normal = normalize(normal_color * vec3( 2.0f, 2.0f, 1.0f ) + vec3( -1.0f, -1.0f, 0.0f)); 
   vec3 world_normal = surface_to_world * surface_normal; // tbn

   // First, some vectors we'll use
   vec3 eye_dir = normalize(EYE_POSITION - passWorldPos); // direction to the eye

   light_factor_t lf = CalculateLighting(passWorldPos, 
      eye_dir, 
      world_normal, 
      SPECULAR_AMOUNT, 
      SPECULAR_POWER); 

   // mix for the debug modes   
   vec4 surface_color = mix( vec4(1, 1, 1, 1), tex_color, RENDER_MIX.x); 
   lf.diffuse = mix( vec3(0), lf.diffuse, RENDER_MIX.y); 
   lf.specular = mix( vec3(0), lf.specular, RENDER_MIX.z); 

   // Add color of the lit surface to the reflected light
   // to get teh final color; 
   vec4 final_color = vec4(lf.diffuse, 1) * surface_color + vec4(lf.specular, 0); 

   final_color = clamp(final_color, vec4(0), vec4(1) ); // not necessary - but overflow should go to bloom target (bloom effect)
   outColor = final_color;

   outColor = ApplyFog(outColor, passViewPos.z);

   // override any color if we're in one of these different modes
   // SUPER inefficient, but whatever, debug
   switch (int(RENDER_MIX.w)) {
      case 1: outColor = vec4( passUV + uv_offset, 0, 1.0f ); break;
      case 2: outColor = vec4( WorldNormalToColor( normalize(passWorldNormal) ), 1);  break;
      case 3: outColor = vec4( WorldNormalToColor( normalize(world_vtan) ), 1); break;
      case 4: outColor = vec4( WorldNormalToColor( normalize(world_vbitan) ), 1);  break;
      case 5: outColor = vec4( normal_color, 1 ); break;
      case 6: outColor = vec4( WorldNormalToColor( normalize(world_normal) ), 1); break;
      default: if (RENDER_MIX == vec4(1,0,0,0)) {
         outColor = tex_color; 
      } break; 
   }

   //vec3 debug_color = (surface_normal + vec3(1)) * 0.5f;
   //outColor = vec4(debug_color, 1);
}