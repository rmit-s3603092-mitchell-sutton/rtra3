//shader.vert

varying vec3 colour;
varying float upDown;
varying float nextupdown;
varying vec3 tempn;
varying float noise;
varying float colSphere;

varying vec4 osVert;
varying vec4 esVert;
varying vec4 csVert;
varying vec3 nEC;


uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;


void main(void)
{

  nEC = normalMatrix * normalize(gl_Normal);
  osVert = gl_Vertex;
  esVert = modelViewMatrix * osVert;
//	esVert = gl_Vertex;
  csVert = gl_ProjectionMatrix * esVert;
  gl_Position = csVert;


}
