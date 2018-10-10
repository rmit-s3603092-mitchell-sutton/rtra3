//shader.frag

varying vec3 colour;

varying vec4 osVert;
varying vec4 esVert;
varying vec4 csVert;
uniform float time;

void main (void)
{
	
	float depth = sqrt((450.0-gl_FragCoord.x)*(450.0-gl_FragCoord.x) + (450.0-gl_FragCoord.y)*(450.0-gl_FragCoord.y));
	float r = sin(depth/50.0-time*time/50.0) + 0.5;
	float g = sin(depth/50.0+2.094-time*time/50.0) + 0.5;
	float b = sin(depth/50.0+4.188-time*time/50.0) +0.5;
	float additive = (1.5 - time/50.0);
	if(additive < 0.0) additive = 0.0;
	r += additive;
	g += additive;
	b += additive;
  	gl_FragColor = vec4(vec3(r,g,b), 1);
}
