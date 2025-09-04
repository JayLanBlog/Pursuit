#version 330       
in vec2 fragTexCoord;              


in vec3 fragNormal;          
in vec3 FragPos;


layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;



uniform sampler2D texture0;        
uniform vec4 colDiffuse;           
void main()                        
{                                  
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(fragNormal);
    // and the diffuse per-fragment color
    gAlbedo.rgb = vec3(0.95);
}                                  





