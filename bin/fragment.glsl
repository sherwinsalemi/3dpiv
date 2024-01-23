#version 330

out vec4 color;

in vec2 TexCoord;

uniform sampler2D uTexture;

void main() {
	color = texture(uTexture, TexCoord);
}