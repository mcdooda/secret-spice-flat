#version 130

uniform vec4 color;

out vec4 outColor;

in vec2 uv2;

void main()
{
	float a;
	if (uv2.x < 0.05 || uv2.x > 0.95 || uv2.y < 0.05 || uv2.y > 0.95)
		a = 0.5;
	else
	{
		float l = length((uv2 - vec2(0.5, 0.5)) * 2);
		a = (l * l / (1.4 * 1.4)) * 0.3 + 0.7;
	}
	outColor = vec4(color.rgb * a, color.w);
}


