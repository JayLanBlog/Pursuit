#version 330 
in vec2 fragTexCoord;  
in vec4 fragColor;
out vec4 finalColor; 
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

#define ITERATION 350
#define GOLDENROT (2.39996) //137 2.39996 0.381966
mat2 rot = mat2(cos(GOLDENROT), sin(GOLDENROT), -sin(GOLDENROT), cos(GOLDENROT));

vec4 BokenBlur(vec2 uv){
    vec4 accumulator;
    vec4 divisor;
    float r = 1.0;
    float RADIUS = 2. * sin(u_time /3.0);
    float PIEXL = 1.0 / u_resolution.x;
    vec2 angle = vec2(0.0, RADIUS);
    for(int j  = 0; j < ITERATION ; j++){
          r += 1.0 / r;
          angle = rot * angle;
          vec4 boken = texture(texture0,uv+PIEXL* (r -1.0) * angle);
          accumulator += boken * boken;
          divisor += boken;
    }
     accumulator = accumulator / divisor;
     return accumulator;
}


void main(){
     finalColor = BokenBlur(fragTexCoord); 
}