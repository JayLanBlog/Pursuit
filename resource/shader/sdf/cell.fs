#version 330
uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;


out vec4 finalColor;
vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

    vec3 color = vec3(0.);
   // color = vec3(st.x,st.y,abs(sin(u_time)));

     // Scale
    st *= 3.;
	   // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);
	
    vec2 point = random2(i_st);
    vec2 diff = point - f_st;

    
      float dist = length(diff);

      color += dist;

        // Draw cell center
		color += 1.-step(.02, dist);

        // Draw grid
     color.r += step(.98, f_st.x) + step(.98, f_st.y);

    finalColor = vec4(color,1.0);
}