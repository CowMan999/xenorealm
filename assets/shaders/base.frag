#ifdef GL_ES
precision mediump float;
#endif
		
uniform vec2 pos;
uniform vec2 winpos;
uniform vec2 res;
uniform float time;
uniform sampler2D tex;
		
void main() {
	gl_FragColor = texture(tex, vec2(gl_FragCoord.x - winpos.x, winpos.y - gl_FragCoord.y)/vec2(32,32)) * vec4(brightness, brightness, brightness, 1.0);
}