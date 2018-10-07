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
	float r = sin(32.0*depth)+0.5;
	float g = sin(32.0*depth+2.0)+0.5;
	float b = sin(32.0*depth+4.0)+0.5;


  //gl_FragColor = vec4(vec3(3.142*sin(36.0*depth),3.142*sin(32.0*depth+2.0),3.142*sin(28.0*depth+4.0)), 1); looks kinda like an explosion
    gl_FragColor = vec4(vec3(r,g,b), 1.0); 
}
