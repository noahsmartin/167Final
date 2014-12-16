#version 120

/*uniform sampler2D normal_texture;
 varying vec3 TexCoord0;
 uniform mat4x4 ModelMatrix;
 
 #define PI 3.141592654
 #define TWOPI 6.283185308
 
 void main() {
 vec3 coord = normalize(TexCoord0);
 float radius = 1;
 float x = coord.x;
 float y = coord.z;
 float t = acos(coord.y/radius)/PI;
 float s;
 
 if (y >= 0)
 s = acos(x/(radius * sin(PI*(t)))) / TWOPI;
 else
 s = (PI + acos(x/(radius * sin(PI*(t))))) / TWOPI;
 
 // Extract the normal from the normal map
 vec3 normal = normalize(texture2D(normal_texture, vec2(s,t)).rgb * 2.0 - 1.0);
 /* mat4x4 ModelMatrix;
 
 ModelMatrix[0] = vec4(0.707, 0, 0.707, 0);
 ModelMatrix[1] = vec4(0, 1, 0, 0);
 ModelMatrix[2] = vec4(-0.707, 0, 0.707, 0);
 ModelMatrix[3] = vec4(0, 0, 0, 1);*/


// Determine where the light is positioned (this can be set however you like)
/*   vec3 lightPos = vec3(0, 0, 1);
 normal = vec3(gl_NormalMatrix * normal);
 normal = normalize(normal);
 lightPos = normalize(lightPos);
 
 // Calculate the lighting diffuse value
 float diffuse = max(dot(normal, lightPos), 0.0);
 
 vec3 color = diffuse * vec3(0.5, 0.5, 0.5);
 
 // Set the output color of our current pixel
 gl_FragColor = vec4(color, 1.0);
 }*/


#define PI 3.141592654
#define TWOPI 6.283185308

varying vec3 TexCoord0;

uniform sampler2D normal_texture;

// New bumpmapping
varying vec3 lightVec;
varying vec3 halfVec;
varying vec3 eyeVec;
varying vec4 position;
varying vec3 vVertex;
varying vec3 normal_t;

// shadow stuff
uniform sampler2D ShadowMap;
varying vec4 ShadowCoord;

varying vec4 diffuse,ambientGlobal, ambient;
varying vec3 normal,halfVector1;

void main()
{
    vec3 n,viewV, halfV, lightDir;
    float NdotL,NdotHV;
    vec4 color = gl_Color;
    float att, dist;
    
    n = normalize(normal_t);
    
    // Compute the ligt direction
    lightDir = vec3(gl_LightSource[0].position-position);
    
    dist = length(lightDir);
 
    
    NdotL = max(dot(n,normalize(lightDir)),0.0);
 
    if (NdotL > 0.0) {
     
        att = 1.0 / (gl_LightSource[0].constantAttenuation +
                gl_LightSource[0].linearAttenuation * dist +
                gl_LightSource[0].quadraticAttenuation * dist * dist);
        color += att * (diffuse * NdotL + ambient);
     
         
        halfV = normalize(halfVector1);
        NdotHV = max(dot(n,halfV),0.0);
        color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
    }
 
    //gl_FragColor = color;

    vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
    
    // Used to lower moiré pattern and self-shadowing
    shadowCoordinateWdivide.z += 0.0005 - 0.000555 ;
    
    float distanceFromLight = texture2D(ShadowMap,shadowCoordinateWdivide.st).z;
    
    
    float shadow = 1.0;
    if (ShadowCoord.w > 0.0)
        shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.25 : 1.0 ;
    
    color = vec4(1.0, 0, 0, 0);
    gl_FragColor = color;
    gl_FragColor *= shadow;


    // bump mapping 
    vec3 coord = normalize(TexCoord0);
    float radius = 1;
    float x = coord.x;
    float y = coord.z;
    float t = acos(coord.y/radius)/PI;
    float s;
    
    if (y >= 0)
        s = acos(x/(radius * sin(PI*(t)))) / TWOPI;
    else
        s = (PI + acos(x/(radius * sin(PI*(t))))) / TWOPI;
    // lookup normal from normal map, move from [0,1] to  [-1, 1] range, normalize
    vec3 normal = 2.0 * texture2D (normal_texture, vec2(s, t)).rgb - 1.0;
    normal = normalize (normal);
    
    // compute diffuse lighting
    float lamberFactor= max (dot (lightVec, normal), 0.0) ;
    vec4 diffuseMaterial = vec4(0.5, 0.5, 0.5, 1);
    
    // compute ambient
    
    
    gl_FragColor =	diffuseMaterial * lamberFactor;
    gl_FragColor += vec4(0.05, 0.05, 0.05, 0); 

    //gl_FragColor = vec4(1.0, 0, 0, 0);
    gl_FragColor *= shadow;

    // toon shading - https://github.com/mchamberlain/Cel-Shader/tree/master/shaders

/*
    vec4 light_pos, light_dir, eye_dir, reflect_dir;
    light_pos = gl_LightSource[0].position;
    light_dir = -vec4(vec3(gl_LightSource[0].position-position), 0);
    eye_dir = normalize(-position);
    reflect_dir = normalize(reflect(light_dir, vec4(normal.xyz, 0)));
    
     float spec = max(dot(reflect_dir, vec4(eye_dir.xyz, 0)), 0.0);
     float diffuse = max(dot(-light_dir, vec4(normal.xyz, 0)), 0.0);
     
     float intensity;
     
     intensity = 0.6 * diffuse + 0.4 * spec;
     // intensity = 0.4 * spec;
     
     if (intensity > 0.9) {
       intensity = 1.1;
       }
       else if (intensity > 0.5) {
       intensity = 0.7;
       }
       else {
       intensity = 0.5;
     }

  vec3 eyePos, lightPos, EyeVert, LightVert, EyeLight;

  eyePos = vec3(0.0,0.0,2.0);
  lightPos = vec3(light_pos.xyz);
  EyeVert = normalize(eyePos - vVertex);
  LightVert = normalize(lightPos - vVertex);
  EyeLight = normalize(LightVert+EyeVert);
  // Simple Silhouette
  float sil;
  sil = max(dot(-normal_t,EyeVert), 0.0);
  
  if (sil < 0.1) {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
  else {
    gl_FragColor = diffuseMaterial * lamberFactor * intensity * shadow;
  }*/
    
}

