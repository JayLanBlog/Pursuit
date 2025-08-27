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
    vec4 sum = texture(texture0, uv) * 4.0;
    sum += texture(texture0, uv - halfpixel.xy * offset);
    sum += texture(texture0, uv + halfpixel.xy * offset);
    sum += texture(texture0, uv + vec2(halfpixel.x, -halfpixel.y) * offset);
    sum += texture(texture0, uv - vec2(halfpixel.x, -halfpixel.y) * offset);
    return sum / 8.0;
}
void main(){
     finalColor = sumcolor(fragTexCoord); 
}