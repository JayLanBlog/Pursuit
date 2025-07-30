#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 offsets[100];

out vec3 ourColor;
out vec2 TexCoord;

void main(){
    vec3 offset = offsets[gl_InstanceID]* (gl_InstanceID / 100.0);;
   // aPos.xy =  aPos.xy + offset;
    gl_Position =projection*view*model*vec4(aPos , 1.0);
    gl_Position.xyz -= offset;
    ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

}