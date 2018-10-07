//shader.frag

varying vec3 colour;

varying vec4 osVert;
varying vec4 esVert;
varying vec4 csVert;
uniform float time;

void main (void)
{
	
	float depth = gl_FragCoord.y;
	float r =sin(depth/50.0+time*sin(time))+0.5;
	float g = sin(depth/50.0+2.094+time*sin(time))+0.5;
	float b =sin(depth/50.0+4.188+time*sin(time))+0.5;
  	gl_FragColor = vec4(vec3(r,g,b), 1);
}
