#version 330 core
in vec3 vertexPosition;
in vec2 vertexTexCoord; 
in vec4 vertexColor;

out vec2 fragTexCoord;
out vec4 fragColor;
uniform mat4 mvp;
uniform float Time;

uniform mat4 model;

//begin choose
out vec3 Normal;
out vec3 FragPos;
out float Height;

// Functions headers
float noise(float i);
vec2 rotate(vec2 v, float a);
vec2 rotateDir(vec2 dir, float seed);

// UI Uniforms
uniform int   WaveCount;
uniform float Amp;
uniform float Frequency;
uniform float Steepness;
uniform float MAX_ANGLE;
uniform vec2  Direction;
//end choose

void main(){
    // Setup variables for sum of waves
	vec3 inDirect = vec3(Direction.x, 0.0, Direction.y);

    inDirect = normalize(inDirect);
    vec3 Position = vertexPosition;
	float t = Time;
    Position.y = 0.0;

    vec3 Normal = vec3(0.0);
    float Step = Steepness;
	float Amplitude  = Amp;
	float Speed = Frequency;
	float TOTAL_AMP = 0.0;

    for(int i=0; i<WaveCount; i++){
        // Wave parameters
		float l = float(i);
		float p = .5*(l+2.0);
        
        // Increment for output var Height
		TOTAL_AMP += Amplitude;

        // Random Direction （Y）
		vec2 randomDir = rotateDir(inDirect.xz, l);
        vec3 Direct = vec3(randomDir.x, 0.0, randomDir.y);
		Direct= normalize(Direct);
        // Setup
        float steepAmp = Step * Amplitude;
		float inner =  Speed * dot(Direct.xz, Position.xz)+p*t;
        // Geometry
        Position.x += Step * Amplitude * Direct.x * cos(inner);
        Position.z += Step * Amplitude * Direct.z * cos(inner);
        Position.y += Amplitude * sin(inner);
        // Normal
		float O = Speed * dot(Direct,Position)+p*t;
        float WaterAmplitude = Speed*Amplitude;
        float CO = cos(O);
        Normal.x += Direct.x * WaterAmplitude * CO; 
        Normal.z += Direct.z * WaterAmplitude * CO; 
        Normal.y += Step * WaterAmplitude * sin(O);
		// FBM :: To Prevent Values from getting too large (a, Qi) and increase frequency to have finer details as wave count increases
		Amplitude *= 0.81;
		Speed  *= 1.18;
		Step *= 0.9;
    }

    // Output Normal
    Normal.x *= -1.0;
    Normal.z *= -1.0;
    Normal.y = 1.0 - Normal.y;
	Normal = normalize(Normal);
    // Should be range [0,1] for lerping purposes in fragment shader
	Height  = (Position.y+TOTAL_AMP) / (TOTAL_AMP*2.0);
    FragPos = vec3(model * vec4(Position, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor; 
    gl_Position = mvp*vec4(Position, 1.0); 
}

float noise(float i)
{
	return fract( sin(i)*43758.5453123 );
}

vec2 rotate(vec2 v, float a)
{
	return vec2( v.x*cos(a) - v.y*sin(a) ,
				 v.x*sin(a) + v.y*cos(a) );
}

vec2 rotateDir(vec2 dir, float seed)
{
	float r = ( noise(seed)*2.0 ) - 1.0;
	float angle = r*MAX_ANGLE;
	return rotate(dir, radians(angle));
}