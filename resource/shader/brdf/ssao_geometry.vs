#version 330                       
in vec3 vertexPosition;    
in vec3 vertexNormal;            
in vec2 vertexTexCoord;
in vec4 vertexColor;


out vec3 fragNormal;          
out vec2 fragTexCoord;             
out vec3 FragPos;
uniform mat4 mvp;  



uniform bool invertedNormals;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()                        
{                                  
    fragTexCoord = vertexTexCoord; 

    vec4 viewPos = view*model*vec4(vertexPosition, 1.0); 
    FragPos = viewPos.xyz;
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    fragNormal = normalMatrix * (invertedNormals ? -vertexNormal : vertexNormal);
    gl_Position = projection * viewPos;
}         