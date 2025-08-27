#version 330 
in vec2 fragTexCoord;  
in vec4 fragColor;
out vec4 finalColor; 
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;


#define ITERATION (450.)


float rand(vec2 n){
    return sin(dot(n,vec2(1233.224,1743.335)));
}


vec4 GrainyBlur(vec2 uv){
    
    float BLUR_RADIUS = 1. / u_resolution.x * 10. * sin(u_time);
    vec2 randomOffset = vec2(0.);
    vec3 col = vec3(0.);
    float random = rand(uv);

    for(float i = 0.; i < ITERATION; i = i + 1.){
      random = fract(43758.5453 * random + 0.61432);
      randomOffset.x = (random - 0.5) * 2.0;
      random = fract(43758.5453 * random + 0.61432);
      randomOffset.y = (random - 0.5) * 2.0;
      col += texture(texture0, uv + randomOffset * vec2(BLUR_RADIUS)).xyz;
    }
    col = col / ITERATION;
    return vec4(col.xyz,1.0);
}


void main(){
     finalColor = GrainyBlur(fragTexCoord); 
}