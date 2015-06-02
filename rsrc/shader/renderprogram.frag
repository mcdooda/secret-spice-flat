#version 130

#define M_PI 3.1415926535897932384626433832795

uniform float flashValue;
uniform float currentTime;
uniform sampler2D screen;

in vec2 uv2;

out vec4 outColor;

vec4 mult(vec4 a, vec4 b)
{
	return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

void main()
{
	ivec2 screenSize = textureSize(screen, 0);
	vec2 px = vec2(1.0 / screenSize.x, 1.0 / screenSize.y);
	float initAngle = currentTime * 20.0;
	vec3 angles = vec3(initAngle, initAngle + M_PI * 2.0 / 3.0, initAngle - M_PI * 2.0 / 3.0);
	float radius = flashValue * 20.0;
	float high = 1.0 + flashValue * 0.5;
	float low = (1 - high) / 2;
	outColor = mult(texture(screen, uv2 + vec2(cos(angles.r) * px.x, sin(angles.r) * px.y) * radius), vec4(high, low, low, 1.0))
	         + mult(texture(screen, uv2 + vec2(cos(angles.g) * px.x, sin(angles.g) * px.y) * radius), vec4(low, high, low, 1.0))
	         + mult(texture(screen, uv2 + vec2(cos(angles.b) * px.x, sin(angles.b) * px.y) * radius), vec4(low, low, high, 1.0));
}


