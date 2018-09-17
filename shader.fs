//shader.frag

varying vec3 colour;
uniform float shininess;

varying vec4 osVert;
varying vec4 esVert;
varying vec4 csVert;
varying vec3 nEC;
uniform bool perPixel;


uniform mat3 normalMatrix;
vec3 renderOnFS(){
// os - object space, es - eye space, cs - clip space

 


  vec3 color = vec3(0.0);
  

  vec3 La = vec3(0.2);
  vec3 Ma = vec3(0.2);
  vec3 ambient = vec3(La * Ma);
  color += ambient;

  vec3 v3 = vec3(osVert.x,osVert.y,osVert.z);
  vec3 lEC = vec3( 0.5, 0.5, 0.5 );// - v3;

  float dp = dot(nEC, lEC);
  if (dp > 0.0) {

    vec3 Ld = vec3(1.0);
    vec3 Md = vec3(0.0,0.5,0.5);

    vec3 nECuse = normalize(nEC );
    float NdotL = dot(nECuse, lEC);
    vec3 diffuse = vec3(Ld * Md * NdotL);
    color += diffuse;

    vec3 Ls = vec3(1.0);
    vec3 Ms = vec3(0.8);

    vec3 vEC = vec3(0,0,1);// -v3;

    vec3 H = vec3(lEC + vEC);
    H = normalize(H);
    float NdotH = dot(nECuse, H);
    if (NdotH < 0.0){ NdotH = 0.0;}
    vec3 specular = vec3(Ls * Ms * pow(NdotH, shininess));
    color += specular;
  }
return color;

}
void main (void)
{
vec3 newColour = colour;
	
  if(perPixel) newColour = renderOnFS();


  gl_FragColor.rgb = newColour;
}
