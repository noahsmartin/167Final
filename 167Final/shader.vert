#version 120

varying vec3 TexCoord0;
uniform mat4x4 ModelMatrix;
varying vec3 lightPos;

void main() {
    TexCoord0 = gl_Normal;
    //TexCoord0 = normalize(gl_NormalMatrix * TexCoord0);
   /* mat4x4 ModelMatrix;
     
     ModelMatrix[0] = vec4(10, 0, 0, 0);
     ModelMatrix[1] = vec4(0, 10, 0, 0);
     ModelMatrix[2] = vec4(0, 0, 10, 0);
     ModelMatrix[3] = vec4(0, 0, 0, 1);*/

    //lightPos = vec3(normalize(ModelMatrix * vec4(lightPos.xyz, 0)));
    // Set the position of the current vertex
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
