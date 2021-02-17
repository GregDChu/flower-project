#version 330 core
in vec4 worldPos;
in float zPos;
in vec4 gl_FragCoord;
//in vec3 fragNor;

out vec4 color;
void main()
{
	//vec3 normal = normalize(fragNor);
	color.rgb = vec3(0.1, 0.8, 1);
	color.a = 0.75;
}
