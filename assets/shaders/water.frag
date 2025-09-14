#ifdef GL_ES
precision highp float;
#endif

uniform vec2 pos;
uniform vec2 res;
uniform float time;
uniform sampler2D tex;
uniform float scale;

void main()
{
	vec2 uv = vec2(gl_FragCoord.x + pos.x*scale, gl_FragCoord.y - pos.y*scale)/vec2(32*scale);

	float xoffset = sin(time*1.55*scale + uv.y);

	vec2 pos2 = vec2(uv.x + xoffset, uv.y);
	vec4 color = texture2D(tex, pos2);
	
	gl_FragColor = color;
}