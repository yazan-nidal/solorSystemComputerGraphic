#version 150

in vec4 vPosition;
in vec4 vColor;
out vec4 color;


uniform mat4 model;
uniform mat4 view;
uniform mat4 tran;
uniform mat4 projection;
uniform vec4 oColor;

void main()
{
	
	color = oColor;
	mat4 model1 = tran*model;
	gl_Position = projection*view*model1*vPosition; 
	
}