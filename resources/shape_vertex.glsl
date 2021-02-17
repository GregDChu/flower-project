#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
//layout(location = 2) in vec3 vertTex;

out vec3 relativePos;
out vec3 absolutePos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 R;
out vec4 screenPos;
out vec3 fragNor;
void main()
{
	fragNor = vec4(R * vec4(vertNor, 0.0)).xyz;
	//fragTex=vertTex;
	relativePos = vertPos;
	//vec4 modelPos = M * vec4(vertPos, 1.0);
	absolutePos = (M * vec4(vertPos, 0)).xyz;
	gl_Position = P * V * M * vec4(vertPos, 1.0);
	screenPos = gl_Position;

}
