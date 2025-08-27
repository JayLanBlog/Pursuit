#version 330                       
in vec3 vertexPosition;    
in vec3 vertexNormal;         
in vec2 vertexTexCoord;            
in vec4 vertexColor;    

out vec2 fragTexCoord;   
out vec4 fragColor;  

out vec3 WorldPos;
out vec3 Normal;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 mvp;

void main()                        
{                                  
    fragTexCoord = vertexTexCoord; 
    WorldPos =  vec3(model * vec4(vertexPosition, 1.0));
    fragColor = vertexColor; 
    Normal = normalMatrix * vertexNormal;  
    gl_Position = mvp*vec4(WorldPos, 1.0); 
}                                  
