#version 330 core

layout(location = 0) in vec3 position;

uniform vec3 u_CamPos;
uniform mat4 u_Proj;
uniform mat4 u_View;

out DATA
{
	vec3 pos;
	vec3 camPos;
	mat4 projection;
	mat4 view;
} data_out;

void main()
{
	data_out.pos = position;
	data_out.camPos = u_CamPos;
	data_out.projection = u_Proj;
	data_out.view = u_View;
}