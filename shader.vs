//shader.vert

varying vec3 colour;
varying float upDown;
varying float nextupdown;
varying vec3 tempn;

varying vec4 osVert;
varying vec4 esVert;
varying vec4 csVert;
varying vec3 nEC;

uniform bool flatShade;
uniform float shininess;
uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform bool calculate;
uniform float t;
uniform int waveDim;
uniform bool perPixel;

vec4 drawSineWave(vec4 vert)
{
  const float A1 = 0.25, k1 = 2.0 * 3.1415926, w1 = 0.25;
  const float A2 = 0.25, k2 = 2.0 * 3.1415926, w2 = 0.25;


      if(!flatShade) {tempn = gl_Normal;}
      if(flatShade&&upDown==0.0) nextupdown = (-1.0) * nextupdown + 1.0;
      if(flatShade&&upDown==1.0){
	tempn = gl_Normal;
	upDown=0.0;

      }
      if(flatShade&&upDown==0.0&&nextupdown==1.0) upDown=1.0;
        nEC = tempn;
	vert.y = A1 * sin(k1 * vert.x + w1 * t);
	nEC.x = - A1 * k1 * cos(k1 * vert.x + w1 * t);
   if (waveDim == 3) {

	vert.y = A1 * sin(k1 * vert.x + w1 * t) + A2 * sin(k2 * vert.z + w2 * t);
	nEC.z = - A2 * k2 * cos(k2 * vert.z + w2 * t);

      }



 return vert;



  
}
void renderOnVS(){

  vec3 color = vec3(0.0);
  

  vec3 La = vec3(0.2);
  vec3 Ma = vec3(0.2);
  vec3 ambient = vec3(La * Ma);
  color += ambient;

  vec3 v3 = vec3(csVert.x,csVert.y,csVert.z);
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
    if (NdotH < 0.0)
      NdotH = 0.0;
    vec3 specular = vec3(Ls * Ms * pow(NdotH, shininess));
    color += specular;
  }
 colour = color;
}
void main(void)
{


  osVert = gl_Vertex;
  if(calculate) osVert = drawSineWave(gl_Vertex);
  esVert = modelViewMatrix * osVert;
  csVert = gl_ProjectionMatrix * esVert;
  gl_Position = csVert;
  nEC = normalMatrix * normalize(nEC);

  if(!perPixel) renderOnVS();


}
