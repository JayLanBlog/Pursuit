#version 330 core

out vec4 FragColor;

uniform sampler2D m_diffuse1;
uniform sampler2D diffuse2;
uniform sampler2D diffuse3;

uniform sampler2D m_specular1;
uniform sampler2D specular2;
uniform sampler2D specular3;

uniform sampler2D m_emissive1;

in vec2 fragTexCoord;  

void main()
{
	vec4 emi = texture(m_emissive1, fragTexCoord);
	vec4 diff = texture(m_diffuse1, fragTexCoord);
	FragColor = diff+emi;
}