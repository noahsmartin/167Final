uniform sampler2D ShadowMap;
varying vec4 ShadowCoord;

varying vec4 diffuse,ambientGlobal, ambient, position;
varying vec3 normal,halfVector;
 

varying vec3 vVertex;

void main()
{
    vec3 n,halfV,viewV,lightDir;
    float NdotL,NdotHV;
    vec4 color = gl_Color;
    float att, dist;
    
    /* a fragment shader can't write a verying variable, hence we need
    a new variable to store the normalized interpolated normal */
    n = normalize(normal);
    
    // Compute the ligt direction
    lightDir = vec3(gl_LightSource[0].position-position);
    
    /* compute the distance to the light source to a varying variable*/
    dist = length(lightDir);
 
    
    /* compute the dot product between normal and ldir */
    NdotL = max(dot(n,normalize(lightDir)),0.0);
 
    if (NdotL > 0.0) {
     
        att = 1.0 / (gl_LightSource[0].constantAttenuation +
                gl_LightSource[0].linearAttenuation * dist +
                gl_LightSource[0].quadraticAttenuation * dist * dist);
        color += att * (diffuse * NdotL + ambient);
     
         
        halfV = normalize(halfVector);
        NdotHV = max(dot(n,halfV),0.0);
        color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
    }
 
    //gl_FragColor = color;

	vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
	
	// Used to lower moiré pattern and self-shadowing
	shadowCoordinateWdivide.z += 0.0005;
	
	float distanceFromLight = texture2D(ShadowMap,shadowCoordinateWdivide.st).z;
	
	
 	float shadow = 1.0;
 	if (ShadowCoord.w > 0.0)
 		shadow = distanceFromLight < shadowCoordinateWdivide.z - 0.000555 ? 0.25 : 1.0 ;
  	
  	gl_FragColor = shadow * color;


    // toon shading - https://github.com/mchamberlain/Cel-Shader/tree/master/shaders

    vec4 light_pos, light_dir, eye_dir, reflect_dir;
    light_pos = gl_LightSource[0].position;
    light_dir = -vec4(lightDir.xyz, 0);
    eye_dir = normalize(-position);
    reflect_dir = normalize(reflect(light_dir, vec4(n.xyz, 0)));
    
     float spec = max(dot(reflect_dir, vec4(eye_dir.xyz, 0)), 0.0);
     float diffuse = max(dot(-light_dir, vec4(n.xyz, 0)), 0.0);
     
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
      

    gl_FragColor = shadow * color * intensity; 


  vec3 eyePos, lightPos, EyeVert, LightVert, EyeLight;

  eyePos = vec3(0.0,0.0,10.0);
  lightPos = vec3(light_pos.xyz);
  EyeVert = normalize(eyePos - vVertex);
  LightVert = normalize(lightPos - vVertex);
  EyeLight = normalize(LightVert+EyeVert);
  // Simple Silhouette
  float sil;
  sil = max(dot(n,EyeVert), 0.0);
  
  if (sil < 0.1) {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
  else {
    gl_FragColor = shadow * color * intensity;
    // gl_FragColor = color * intensity;
    
  }

// uncomment the next line to disable toon shading
// gl_FragColor = shadow * color;
}