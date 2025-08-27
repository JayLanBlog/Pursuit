#version 330 
in vec2 fragTexCoord;  
in vec4 fragColor;
out vec4 finalColor; 
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

#define  PI (3.14156)
#define ANGLE (PI * 1. * cos(u_time / 3.))
#define BLUR_RADIUS (5. * sin(u_time))
#define ITERATION (100.)

vec2 getDirection()
{
    float sinVal = (sin(ANGLE) * BLUR_RADIUS * 0.05f) / ITERATION;
    float cosVal = (cos(ANGLE) * BLUR_RADIUS * 0.05f) / ITERATION;
    return vec2(sinVal, cosVal);
}

vec3 DirectionalBlur(vec2 uv)
{
    vec3 col;
    vec2 dir = getDirection();
    
    for(float k = -1. * ITERATION; k < ITERATION; k = k + 1.)
    {
        col += texture(texture0, uv - dir * k).xyz;
    }
    
    col = col / (ITERATION * 2.);
    return col;
}

void main(){
     finalColor = vec4(DirectionalBlur(fragTexCoord).xyz ,1.0); 
}