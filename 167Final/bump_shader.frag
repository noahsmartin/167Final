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


void main()
{
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
    
    
    gl_FragColor =	diffuseMaterial * lamberFactor ;
    gl_FragColor += vec4(0.05, 0.05, 0.05, 0);
    
}

