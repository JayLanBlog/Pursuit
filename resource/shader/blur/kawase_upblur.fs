#version 330 
in vec2 fragTexCoord;  
in vec4 fragColor;
out vec4 finalColor; 
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 iResolution;
uniform vec2 offset;
uniform vec2 halfpixel;

vec4 sumcolor(vec2 uv){
    vec4 sum = texture(texture0, uv + vec2(-halfpixel.x * 2.0, 0.0) * offset);
    sum += texture(texture0, uv + vec2(-halfpixel.x, halfpixel.y) * offset) * 2.0;
    sum += texture(texture0, uv + vec2(0.0, halfpixel.y * 2.0) * offset);
    sum += texture(texture0, uv + vec2(halfpixel.x, halfpixel.y) * offset) * 2.0;
    sum += texture(texture0, uv + vec2(halfpixel.x * 2.0, 0.0) * offset);
    sum += texture(texture0, uv + vec2(halfpixel.x, -halfpixel.y) * offset) * 2.0;
    sum += texture(texture0, uv + vec2(0.0, -halfpixel.y * 2.0) * offset);
    sum += texture(texture0, uv + vec2(-halfpixel.x, -halfpixel.y) * offset) * 2.0;
    return sum / 12.0;
}

void main(){

     finalColor = sumcolor(fragTexCoord); 
}