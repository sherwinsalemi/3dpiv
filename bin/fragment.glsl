#version 330

out vec4 color;

in vec2 TexCoord;

uniform sampler2D uTexture;

void main() {
	color = texture(uTexture, TexCoord);
		if (color.r < 0.03)
	{
		discard;
	}
	// color.a = 0.4;

}