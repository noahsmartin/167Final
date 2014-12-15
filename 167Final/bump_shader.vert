#version 120

/*varying vec3 TexCoord0;
 uniform mat4x4 ModelMatrix;
 
 void main() {
 TexCoord0 = gl_Normal;
 //TexCoord0 = normalize(gl_NormalMatrix * TexCoord0);
 /* mat4x4 ModelMatrix;
 
 ModelMatrix[0] = vec4(10, 0, 0, 0);
 ModelMatrix[1] = vec4(0, 10, 0, 0);
 ModelMatrix[2] = vec4(0, 0, 10, 0);
 ModelMatrix[3] = vec4(0, 0, 0, 1);*/

//lightPos = vec3(0, 0, 1);
//lightPos = vec3(normalize(ModelMatrix * vec4(lightPos.xyz, 0)));
// Set the position of the current vertex
/*  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 }
 */

attribute vec3 tangent;
varying vec3 lightVec;
varying vec3 halfVec;
varying vec3 eyeVec;
varying vec3 TexCoord0;
varying vec4 position;
varying vec3 vVertex;
varying vec3 normal_t;

void main()
{
    normal_t = normalize(gl_NormalMatrix * gl_Normal);
    TexCoord0 = gl_Normal;
    
    // Building the matrix Eye Space -> Tangent Space
    vec3 n = normalize (gl_NormalMatrix * gl_Normal);
    vec3 t = normalize (gl_NormalMatrix * tangent);
    vec3 b = cross (n, t);
    
    vec3 pos;
    pos = vec3(gl_LightSource[0].position);
    
    vec3 vertexPosition = vec3(gl_ModelViewMatrix *  gl_Vertex);
    vec3 lightDir;
    lightDir = normalize(pos.xyz - vertexPosition);
    
    
    // transform light and half angle vectors by tangent basis
    vec3 v;
    v.x = dot (lightDir, t);
    v.y = dot (lightDir, b);
    v.z = dot (lightDir, n);
    lightVec = normalize (v);
    
    
    v.x = dot (vertexPosition, t);
    v.y = dot (vertexPosition, b);
    v.z = dot (vertexPosition, n);
    eyeVec = normalize (v);
    
    
    vertexPosition = normalize(vertexPosition);
    
    /* Normalize the halfVector to pass it to the fragment shader */
    
    // No need to divide by two, the result is normalized anyway.
    // vec3 halfVector = normalize((vertexPosition + lightDir) / 2.0);
    vec3 halfVector = normalize(vertexPosition + lightDir);
    v.x = dot (halfVector, t);
    v.y = dot (halfVector, b);
    v.z = dot (halfVector, n);
    
    // No need to normalize, t,b,n and halfVector are normal vectors.
    //normalize (v);
    halfVec = v ;
    
    
    gl_Position = ftransform();


    position = gl_ModelViewMatrix * gl_Vertex;
    vVertex = gl_Vertex.xyz;
}