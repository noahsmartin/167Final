// Used for shadow lookup
varying vec4 ShadowCoord;

varying vec4 diffuse,ambientGlobal,ambient, position;
varying vec3 normal,halfVector;

varying vec3 vVertex;
 
void main()
{   
    vec3 aux;
     
    /* first transform the normal into eye space and normalize the result */
    normal = normalize(gl_NormalMatrix * gl_Normal);
 
    /* compute the vertex position  in camera space. */
    position = gl_ModelViewMatrix * gl_Vertex;
 
    /* Normalize the halfVector to pass it to the fragment shader */
    halfVector = gl_LightSource[0].halfVector.xyz;
     
    /* Compute the diffuse, ambient and globalAmbient terms */
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

    ShadowCoord = gl_TextureMatrix[7] * gl_Vertex;
  
	gl_Position = ftransform();

    // toon shading 
	gl_FrontColor = gl_Color;
    position = gl_ModelViewMatrix * gl_Vertex;
    vVertex = gl_Vertex.xyz;
}