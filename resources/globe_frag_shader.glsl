#version 330 core
uniform vec3 cam;
in vec4 worldPos;
in float zPos;
in vec4 gl_FragCoord;
in vec3 fragNor;
in vec3 vertex_pos;
out vec4 color;
void main()
{
	vec3 normal = normalize(fragNor);
	vec3 light = vec3(-10, 10, 10);
	vec3 light_vec = normalize(light - vertex_pos);
	float diffuse = dot(normal, light_vec);

	vec3 cam_vec = normalize(vertex_pos - cam);
	vec3 half_vec = normalize(cam_vec + light_vec);
	float spec = dot(normal, half_vec);
	spec = clamp(spec, 0, 1);
	spec = pow(spec, 10);
	
	if(zPos < 0.3){
		discard;
	}else{
		color.rgb = vec3(0.0, 1.0, 1.0);
		color.rgb += vec3(1, 1, 1) * spec * 0.5;
		color.a = 1 - pow((abs(zPos) / 3.0), 0.3);
		color.a += spec * 0.8;
	}
}
