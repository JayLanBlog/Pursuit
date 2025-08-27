#version 330 
in vec2 fragTexCoord;  
in vec4 fragColor;
out vec4 finalColor; 
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float bulrSize;


const int SAMPLE_COUNT = 6;

const float OFFSETS[6] = float[6](
    -4.378621204796657,
    -2.431625915613778,
    -0.4862426846689485,
    1.4588111840004858,
    3.4048471718931532,
    5
);

const float WEIGHTS[6] = float[6](
    0.09461172151436463,
    0.20023097066826712,
    0.2760751120037518,
    0.24804559825032563,
    0.14521459357563646,
    0.035822003987654526
);

// blurDirection is:
//     vec2(1,0) for horizontal pass
//     vec2(0,1) for vertical pass
// The sourceTexture to be blurred MUST use linear filtering!
// pixelCoord is in [0..1]
vec4 blur(in sampler2D sourceTexture, vec2 blurDirection, vec2 pixelCoord)
{
    vec4 result = vec4(0.0);
    vec2 size = textureSize(sourceTexture, 0);
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        vec2 offset = blurDirection * bulrSize* OFFSETS[i] / size;
        float weight = WEIGHTS[i];
        result += texture(sourceTexture, pixelCoord + offset) * weight;
    }
    return result;
}


uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

vec4 GaussionBlur3(vec2 uv){
 vec4 color = texture2D(texture0, vec2(uv)) * weight[0];
  for (int i=1; i<3; i++) {
        color +=
            texture2D(texture0, (uv + vec2(0.0, offset[i])))
                * weight[i];
        color +=
            texture2D(texture0, (uv - vec2(0.0, offset[i])))
                * weight[i];
    }
    return color;

}


vec4 GaussionBlur2(vec2 uv){
    float offset = bulrSize * 0.0625;
    vec4 color = texture(texture0,vec2(uv.x - offset,uv.y - offset));
      
    color += texture(texture0,vec2(uv.x,uv.y - offset)) *  0.40;
    // 右上
    color += texture(texture0,vec2(uv.x + offset,uv.y + offset)) * 0.15f;
    // 左
    color += texture(texture0,vec2(uv.x - offset,uv.y)) * 0.15f;
    // 中
    color += texture(texture0,vec2(uv.x,uv.y)) * 0.10f;
    // 右
    color += texture(texture0,vec2(uv.x + offset,uv.y))  * 0.10f;
    // 左下
    color += texture(texture0,vec2(uv.x - offset,uv.y + offset)) * 0.05;
    // 下
    color += texture(texture0,vec2(uv.x,uv.y + offset)) *0.05f;
    // 右下
    color += texture(texture0,vec2(uv.x + offset,uv.y - offset))*0.05f;
    return color;
}


vec4 GaussionBlur(vec2 uv){
    float offset = bulrSize * 0.0625;
    vec4 color = texture(texture0,vec2(uv.x - offset,uv.y - offset))* 0.0947416f;
      
    color += texture(texture0,vec2(uv.x,uv.y - offset)) * 0.118318f;
    // 右上
    color += texture(texture0,vec2(uv.x + offset,uv.y + offset)) * 0.0947416f;
    // 左
    color += texture(texture0,vec2(uv.x - offset,uv.y)) * 0.118318f;
    // 中
    color += texture(texture0,vec2(uv.x,uv.y)) * 0.147761f;
    // 右
    color += texture(texture0,vec2(uv.x + offset,uv.y)) * 0.118318f;
    // 左下
    color += texture(texture0,vec2(uv.x - offset,uv.y + offset)) * 0.0947416f;
    // 下
    color += texture(texture0,vec2(uv.x,uv.y + offset)) * 0.118318f;
    // 右下
    color += texture(texture0,vec2(uv.x + offset,uv.y - offset)) * 0.0947416f;
    return color;
}


void main(){
    //vec4 texelColor = texture(texture0, fragTexCoord);  
    // finalColor = GaussionBlur3(fragTexCoord); 
   vec4 bulH = blur(texture0,vec2(1.0, 0.0),fragTexCoord);
   vec4 bulV = blur(texture0,vec2(0.0, 1.0),fragTexCoord);

   finalColor = (bulH + bulV)*0.5;
   // finalColor += blur(texture0,vec2(0.0,1.0),fragTexCoord);
}