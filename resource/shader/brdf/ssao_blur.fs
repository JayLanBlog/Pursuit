#version 330       
in vec2 fragTexCoord;              


out float finalColor;               
uniform sampler2D ssaoInput;        

void main()                        
{                 
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;

      for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, fragTexCoord + offset).r;
        }
    }
    finalColor = result / (4.0 * 4.0);

 //   vec4 texelColor = texture(texture0, fragTexCoord);   
  //  finalColor = texelColor*colDiffuse*fragColor;     
   // finalColor = vec4(1.0,0.0,0.0,1.0);   
}                                  
