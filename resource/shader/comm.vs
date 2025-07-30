#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
uniform mat4 m_mvps[50];
uniform mat4 m_models[50];


out vec3 ourColor;
out vec2 TexCoord;

void main(){
	mat4 mvp = m_mvps[gl_InstanceID];
    mat4 model = m_models[gl_InstanceID];
    gl_Position=mvp* vec4(aPos, 1.0);
    ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

}