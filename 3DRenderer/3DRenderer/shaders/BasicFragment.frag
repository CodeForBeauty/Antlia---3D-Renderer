#version 330 core

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
out vec4 color;

void main()
{
	color = texture(u_Texture, v_TexCoord);//vec4(0.1, 0.0, 1.0, 1.0);
}