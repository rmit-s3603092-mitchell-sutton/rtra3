//shader.frag

varying vec3 colour;
uniform float shininess;

varying vec4 osVert;
varying vec4 esVert;
varying vec4 csVert;
varying vec3 nEC;
uniform bool perPixel;

void main (void)
{
	float depth = gl_FragCoord.z;
	


  gl_FragColor = vec4(vec3(3.142*sin(64.0*depth),3.142*sin(64.0*depth+2.0),3.142*sin(64.0*depth+4.0)), 1);
}
