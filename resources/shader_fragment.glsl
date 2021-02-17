#version 330 core
out vec4 color;
in vec3 vertex_color;
in vec3 originalPos;
void main()
{
	float y = originalPos.y;
	color.rgb = (y * vec3(0.1, 0.1, 0.1)) + ((1 - y) * vec3(0.7, 0.7, 0.7));

	color.a=1;	//transparency: 1 .. 100% NOT transparent
}
