#version 330
out vec4 finalColor;           
in vec2 fragTexCoord;              
in vec4 fragColor;

in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 camPos;
const float PI = 3.14159265359;


uniform sampler2D texture0; 


uniform vec4 colDiffuse;   


/*计算法线分布*/
//Dggx(n,h,a) = a * a / PI * ((a*a - 1)(n * h)+1)^2
/*                          a*a
    GGXFunc = ---------------------------------
               PI * ( (a * a -1）* (n * h)+1)^2

    a= avg(roughness)     
*/
float DistributionGGX(vec3 N, vec3 H, float roughness){
     float a = roughness*roughness;
     float a2 = a*a;
     float NdotH = max(dot(N, H), 0.0);
     float NdotH2 = NdotH * NdotH;
     float nom   = a2;
     float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}



/*
    计算几何遮避
*/
/* 
                    N * V
   Func =   ---------------------
             (N * V) *(1 - k )+k



            (roughness + 1) *  (roughness + 1)
        k = -------------------------------
                           8
 */
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

/*
    Geometry遮挡 = Fuc * Func
*/

// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

/*
    反射光照的比例，菲尼尔系数
*/
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()                        
{            
    vec3 N = normalize(Normal);
    //相机指向微面的方向（世界坐标位置）
    vec3 V = normalize(camPos - WorldPos);
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
      // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) {
        // calculate per-light radiance
        //光照指向微面的方向（世界坐标系）
        vec3 L = normalize(lightPositions[i] - WorldPos);
        //相机指向方向和光照指向方向的的半程向量
        vec3 H = normalize(V + L);
        //光照到微面的距离
        float distance = length(lightPositions[i] - WorldPos);
        //面积的1个百分比
        float attenuation = 1.0 / (distance * distance);
       
        vec3 radiance = lightColors[i] * attenuation;


        //NDF 为法线分布函数
        float NDF = DistributionGGX(N, H, roughness);   
        //G 为几何遮蔽
        float G   = GeometrySmith(N, V, L, roughness);      
        //F 光线的反射比例
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        /*
            反射系数计算公式：
                            D * F * G
              Specular = ---------------------
                         4 * (N * L) * (N * L)
        */
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        


          // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

          // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    }

      // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

   // vec4 texelColor = texture(texture0, fragTexCoord);   
    finalColor = vec4(color,1.0);       
}                                  
