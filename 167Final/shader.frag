 #version 120

uniform samplerCube normal_texture;
varying vec3 TexCoord0;

void main() {
    
    // Extract the normal from the normal map
    vec3 normal = normalize(textureCube(normal_texture, TexCoord0).rgb * 2.0 - 1.0);
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
