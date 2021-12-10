
#include <glew.h>
#include <GL/glut.h>
#include "shader.hpp"
#include <cmath>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>



#define N 4  // number of subdivisions
#define M 16*64*3  // number of resulting points
static int k = 0;
glm::vec4 data[M];

glm::vec4 v[4] = { glm::vec4(0.0, 0.0, 1.0, 1.0), glm::vec4(0.0, 0.942809, -0.333333, 1.0),
				glm::vec4(-0.816497, -0.471405, -0.333333, 1.0),
			   glm::vec4(0.816497, -0.471405, -0.333333, 1.0)
};

glm::vec4 unit(const glm::vec4& p)
{
	glm::vec4 c;
	double d = 0.0;
	for (int i = 0; i < 3; i++) d += p[i] * p[i];
	d = sqrt(d);
	if (d > 0.0) for (int i = 0; i < 3; i++) c[i] = p[i] / d;
	c[3] = 1.0;
	return c;
}

void triangle(glm::vec4  a, glm::vec4 b, glm::vec4 c)
{
	data[k] = a;
	k++;
	data[k] = b;
	k++;
	data[k] = c;
	k++;
}

void divide_triangle(glm::vec4 a, glm::vec4 b, glm::vec4 c, int n)
{
	glm::vec4 v1, v2, v3;
	if (n > 0)
	{
		v1 = unit(a + b);
		v2 = unit(a + c);
		v3 = unit(b + c);
		divide_triangle(a, v2, v1, n - 1);
		divide_triangle(c, v3, v2, n - 1);
		divide_triangle(b, v1, v3, n - 1);
		divide_triangle(v1, v2, v3, n - 1);
	}
	else triangle(a, b, c);
}

void tetrahedron(int n)
{
	divide_triangle(v[0], v[1], v[2], n);
	divide_triangle(v[3], v[2], v[1], n);
	divide_triangle(v[0], v[3], v[1], n);
	divide_triangle(v[0], v[3], v[2], n);
}

enum MyEnum
{
	line = GL_LINE_LOOP,
	fill = GL_TRIANGLES
}draw;
int w = 80 * 16;
int h = 80 * 9;
//source https://www.nationalgeographic.org/activity/planetary-size-and-distance-comparison/



float sizeOfPlanets[9] =
{
	5.0f,						//sun
	sizeOfPlanets[0] / 277.0f,	//Mercury
	sizeOfPlanets[0] / 113.0f,	//Venus
	sizeOfPlanets[0] / 108.0f,	//Earth
	sizeOfPlanets[0] / 208.0f,	//Mars
	sizeOfPlanets[0] / 9.680f,	//Jupiter
	sizeOfPlanets[0] / 11.40f,	//Saturn
	sizeOfPlanets[0] / 26.80f,	//Uranus
	sizeOfPlanets[0] / 27.70f	//Neptune
};

//float  AU = 150.0f * sizeOfPlanets[0] / 1.4f;
float  AU = 3.75f * sizeOfPlanets[0] / 1.4f;
float distance[9] =
{
	0.0f,
	0.4f * AU,
	0.7f * AU,
	1.0f * AU,
	1.3f * AU,
	2.5f * AU,
	3.2f * AU,
	4.2f * AU,
	5.0f * AU
};

glm::vec3 planentPositions[] = {
	glm::vec3(distance[0],  0.0f,  0.0f),
	glm::vec3(distance[1],  0.0f, 0.0f),
	glm::vec3(distance[2], 0.0f, 0.0f),
	glm::vec3(distance[3], 0.0f, 0.0f),
	glm::vec3(distance[4], 0.0f, 0.0f),
	glm::vec3(distance[5],  0.0f, 0.0f),
	glm::vec3(distance[6], 0.0f, 0.0f),
	glm::vec3(distance[7],  0.0f, 0.0f),
	glm::vec3(distance[8],  0.0f, 0.0f),
};

void bigAllPlanet(float times)
{
	for (int i = 1; i < 9; i++)
		sizeOfPlanets[i] *= times;
}
void bigSun(float times)
{
	sizeOfPlanets[0] *= times;
}

glm::vec4 vertex_color[] =
{

		glm::vec4(1.0, 1.0, 1.f, 1.0), //white
		glm::vec4(1.0, 0.0, 0.0, 1.0), //red 
		glm::vec4(122.f / 255.f, 112.f / 255.f, 0.0, 1.0), //yellow  rgb(122, 112, 82)	
		glm::vec4(0.0, 0.0, 190.f / 255.f, 1.0), //
		glm::vec4(194.f / 255.f, 153.f / 255.f, 112.f / 255.f, 1.0), //green	rgb(194, 153, 112)
		glm::vec4(236.f / 255.f, 193.f / 255.f, 19.f / 255.f, 1.0), //magenta						(236, 193, 19)
		glm::vec4(115.f / 255.f, 115.f / 255.f, 140.f / 255.f, 1.0),//blacl  rgb(115, 115, 140)
		glm::vec4(121.f / 255.f, 198.f / 255.f, 236.f / 255.f, 1.0), //cyan    rgb(121, 198, 236)
		glm::vec4(42.f / 255.f, 42.f / 255.f, 111.f / 255.f, 1.0), //rgb(42, 42, 111)

};
//------------------------------------------------------------
GLuint model;
GLuint view;
GLuint projection;
GLuint oColor;
GLuint tran;

glm::mat4 modelMat = glm::mat4(1.f);
glm::mat4 viewMat = glm::mat4(1.f);
glm::mat4 projectionMat = glm::mat4(1.f);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 11.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 direction;
float cameraSpeed = 0.15f;
float sensitivity = 0.05;

float yaw = -90.0f;
float pitch = 0.0f;
//--------------------------------------------------------------
bool lockPla = false;
bool lockCam = false;
float ra = 0;

//------------------------------------------------------------

//openGl initialization
void init()
{
	tetrahedron(N);
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffers;
	glGenBuffers(1, &buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	GLuint program = LoadShaders("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	model = glGetUniformLocation(program, "model");

	view = glGetUniformLocation(program, "view");
	projection = glGetUniformLocation(program, "projection");
	oColor = glGetUniformLocation(program, "oColor");
	tran = glGetUniformLocation(program, "tran");

	draw = line;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

}
//----------------------------------------------------------------------------------------------

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < 9; i++) {

		glm::mat4 modelMat1 = glm::mat4(1.f);
		modelMat1 = glm::translate(modelMat1, planentPositions[i]);
		modelMat1 = glm::scale(modelMat1, glm::vec3(sizeOfPlanets[i]));
		glUniformMatrix4fv(tran, 1, GL_FALSE, glm::value_ptr(modelMat1));

		glUniform4fv(oColor, 1, glm::value_ptr(vertex_color[i]));

		for (unsigned int i = 0; i < M; i += 3)
		{
			glDrawArrays(draw, i, 3);
		}
	}

	glutSwapBuffers();
}
//--------------------------------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{

	case 033://Escape key
		exit(0);
		break;
	case 'l':case'L':
		if (draw == line)
			draw = fill;
		else draw = line;
		break;
	case 'Z':case 'z':
		lockPla = !lockPla;
		break;
	case 'C':case 'c':
		lockCam = !lockCam;
		break;
	case 'Q':case 'q':
		ra = 0;
		for (int i = 1; i < 9; i++)
			planentPositions[i] = glm::vec3(distance[i], 0.f, 0.f);
		modelMat = glm::mat4(1.f);
		break;
	case 'E':case 'e':
		cameraPos = glm::vec3(0.0f, 0.0f, 11.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = -90.0f;
		pitch = 0.0f;
		//lockCam = !lockCam;
		break;
	}

	if (key == 'w' || key == 'W')
		cameraPos += cameraSpeed * cameraFront;
	if (key == 's' || key == 'S')
		cameraPos -= cameraSpeed * cameraFront;
	if (key == 'a' || key == 'A')
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == 'd' || key == 'D')
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	glutPostRedisplay();
}
bool firstMouse = true;

int lastX = h / 2;
int lastY = w / 2;

void mov(int x, int y)
{

	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	if (lastX > w - 30)
		lastX -= 30;
	if (lastX < 30)
		lastX += 30;
	if (lastY > h - 30)
		lastY -= 30;
	if (lastY < 30)
		lastY += 30;

	float xoffset = x - lastX;
	float yoffset = lastY - y;
	lastX = x;
	lastY = y;

	if (lockCam)
		return;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
		pitch = -89.0f;



	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	// update up
	glm::vec3 Right = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	cameraUp = glm::normalize(glm::cross(Right, cameraFront));

	glutPostRedisplay();
}
//--------------------------------------------------------------------------------


void idle()
{

	if (!lockPla)
		ra += 0.008;
	if (!lockPla) {
		for (int i = 1; i < 9; i++)
		{
			float camX = sin(ra / distance[i]) * distance[i];
			float camZ = cos(ra / distance[i]) * distance[i];

			planentPositions[i] = glm::vec3(camX, 0.f, camZ);

		}
		modelMat = glm::rotate(modelMat, glm::radians(0.02f), glm::vec3(0.f, 0.f, 1.f));
	}
	viewMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projectionMat = glm::perspective(
		45.f,
		(float)w / (float)h,
		0.1f,
		45.f
	);

	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(modelMat));
	glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projectionMat));
	glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(viewMat));

	glutPostRedisplay();
}
//-------------------------------------------------------------------------------
void reshape(int W, int H)
{
	glViewport(0, 0, W, H);
	w = W;
	h = H;
}
//-------------------------------------------------------------------------------

void mainMenuHandler(int choise)
{
	switch (choise)
	{
	case 0:		//reSet the planet
		ra = 0;
		for (int i = 1; i < 9; i++)
			planentPositions[i] = glm::vec3(distance[i], 0.f, 0.f);
		modelMat = glm::mat4(1.f);
		break;
	case 1:		//reSet the camera
		cameraPos = glm::vec3(0.0f, 0.0f, 11.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = -90.0f;
		pitch = 0.0f;
		break;
	case 2:		//toggel the planet
		lockPla = !lockPla;
		break;
	case 3:		//toggle the cam
		lockCam = !lockCam;
		break;
	case 4:
		if (draw == line)
			draw = fill;
		else draw = line;
		break;
	}
}

void muneSize(int choise)
{
	switch (choise)
	{
	case 0:
		bigAllPlanet(2.f);
		break;
	case 1:
		bigAllPlanet(0.5f);
		break;
	case 2:
		bigSun(1.1f);
		break;
	case 3:
		bigSun(0.9f);
		break;
	}
}
void backgroundColor(int choise)
{
	if (choise == 0)		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	if (choise == 1)		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(w, h);
	glutCreateWindow("colorCube");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mov);
	glutPassiveMotionFunc(mov);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	int subMenu = glutCreateMenu(muneSize);
	glutAddMenuEntry("planet double", 0);
	glutAddMenuEntry("planet half", 1);
	glutAddMenuEntry("sun (1.1) times", 2);
	glutAddMenuEntry("sun (0.9) times", 3);

	glutCreateMenu(mainMenuHandler);
	glutAddSubMenu("sizeChanger", subMenu);
	glutAddMenuEntry("reSet  planet (Q)", 0);
	glutAddMenuEntry("reSet the camera (E)", 1);
	glutAddMenuEntry("toggle planet (Z)", 2);
	glutAddMenuEntry("toggle the camera (C)", 3);
	glutAddMenuEntry("toggle the draw (L)", 4);


	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}
