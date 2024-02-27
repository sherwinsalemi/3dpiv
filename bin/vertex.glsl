#version 330

layout(location = 0) in vec3 v;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 matrix;

void main() {
	gl_Position = matrix * vec4(v, 1.0);
	TexCoord = aTexCoord;
}