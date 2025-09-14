#ifdef GL_ES
precision highp float;
#endif

uniform vec2 pos;
uniform vec2 winpos;
uniform vec2 res;
uniform float time;
uniform sampler2D tex;

void main()
{
	vec2 uv = (vec2(gl_FragCoord.x - winpos.x, winpos.y - gl_FragCoord.y)/vec2(32,32) - vec2(0.5)) * 2;

	float dist = abs(length(uv) - 0.5);
	dist = sin(dist*8 + time*3)/8;
	
	vec4 color = vec4(dist);

	color = 0.02/color;
	color.a = 1.0;

	
	gl_FragColor = color;
}