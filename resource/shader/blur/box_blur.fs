#version 330 
in vec2 fragTexCoord;  
in vec4 fragColor;
out vec4 finalColor; 
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float texelWidthOffset;
uniform float texelHeightOffset;

uniform float BlurKernelSize;// = 3.0;//3.0 or 5
uniform int   BlurDirection;// = 1;// 1.0 or 0

vec4 BoxBlur(int blurKernelSize, vec2 blurRadius, sampler2D texSampler, vec2 texCoord)
{
      vec4 sumValue = vec4(0.0);
      vec2 texCoord2;
      float count = 0.0;

      for (int i = -blurKernelSize; i <= blurKernelSize; i++) {
            texCoord2 = texCoord + blurRadius * float(i);
            
            sumValue += texture2D(texSampler, texCoord2);
            count += 1.0;
      }
      sumValue = sumValue / count;
      return sumValue;
}

void main(){
    vec2 blurDir = vec2(1.0,0.0);
    if(BlurDirection!=0) blurDir = vec2(0.0,1.0);
    vec2 invSize = vec2(texelWidthOffset,texelHeightOffset);
    vec2 blurRadius = invSize * blurDir;
    finalColor = BoxBlur(int(BlurKernelSize), blurRadius, texture0, fragTexCoord);
}