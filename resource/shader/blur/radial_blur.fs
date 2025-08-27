#version 330 
in vec2 fragTexCoord;  
in vec4 fragColor;
out vec4 finalColor; 
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

#define BLUR_RADIUS (1. / u_resolution.x * 1.)
#define ITERATION (45.)

vec2 getCent(){
     return u_mouse.xy / u_resolution.xy;
}

vec3 RadialBlur(vec2 uv){
    vec2 blurV = (getCent() - uv) * BLUR_RADIUS;
    vec3 col;
    for(float j = 0; j < ITERATION; j = j + 1.0){
        col += texture(texture0, uv).xyz;
        uv += blurV;
    }
    return col / ITERATION;
}

void main(){
     finalColor = vec4(RadialBlur(fragTexCoord).xyz ,1.0); 
}