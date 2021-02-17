#version 330 core
out vec4 color;
in vec3 relativePos;
in vec3 absolutePos;
uniform float trait1;
uniform mat4 M;
in vec3 fragNor;
void main()
{
	float lipCurveIntensity = 0.7 + (trait1 * 4);
	float alpha = ((relativePos.y + 1) / 2);
	vec3 normal = normalize(fragNor);

//Bounding the sphere to get a petal shape
	//Top lip of the petal
	if(relativePos.y < (-1 * pow(relativePos.x, 2) / lipCurveIntensity) + (2.0 / 3.0)
	//Inner curve of the sphere
		&& (pow((relativePos.x), 2) + pow(relativePos.y, 2) + pow((relativePos.z + 2), 2)) >= 7
	//Bottom cut off of sphere
		&& relativePos.y > -0.8){
//Set the color using a gradient of dark red to light red
		//color.rgb =  (alpha * vec3(1, 0, 0.1)) + ((1 - alpha) * vec3(0.2, 0, 0));
		color.rgb = (vec3(1, 1, 1) * 0.4) + ((vec3(1, 1, 1) - normal) * 0.6);
		color.a = 0.9;
	}else{
//If the sphere point is located outside of the bounded regions,
//make them transparent
		//color.rgba = vec4(1, 1, 1, 0);
		discard;
	}
}
