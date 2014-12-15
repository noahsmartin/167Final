 #version 120

#define PI 3.141592654
#define TWOPI 6.283185308

uniform sampler2D normal_texture;
varying vec3 TexCoord0;

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
    vec3 normal = normalize(texture2D(normal_texture,  vec2(s, t)).rgb * 2.0 - 1.0);
     /*mat3x3 ModelMatrix;
     
     ModelMatrix[0] = vec3(10, 0, 100);
     ModelMatrix[1] = vec3(0, 10, 0);
     ModelMatrix[2] = vec3(-100, 0, 10);*/
    
    // Determine where the light is positioned (this can be set however you like)
    vec3 lightPos = vec3(0, 0, 1);

    // Calculate the lighting diffuse value
    float diffuse = max(dot(normal, lightPos), 0.0);
    
    vec3 color = diffuse * vec3(0.5, 0.5, 0.5);
    
    // Set the output color of our current pixel
    gl_FragColor = vec4(color, 1.0);
}
