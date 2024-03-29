
#include <glew/glew.h>
#include <freeglut/freeglut.h>
#include <soil/SOIL.h>
#include <iostream>
#include <vector>
#include <memory>

//#include <vld.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ShaderLoader.h"
#include "camera.h"
#include "boid.h"

#include "utils.h"

using namespace std;

enum InputState
{
	INPUT_UP,
	INPUT_DOWN,
	INPUT_UP_FIRST,
	INPUT_DOWN_FIRST
};

InputState MouseState[3];
InputState KeyState[255];

GLuint program;

float deltaTime;
float previousTimeStamp;

void Render();
void Init(int argc, char **argv);
void Update();
void mousePassiveInput(int x, int y);
void MouseClick(int button, int state, int x, int y);
void KeyboardDown(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);

GLuint VBO, VAO, EBO, texture;

int main(int argc, char **argv)
{
	Init(argc, argv);
	glutMainLoop();
	return 0;
}

void Render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	boid::instance().render(program);
	glutSwapBuffers();
}

void Init(int argc, char **argv)
{
	// Setup
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("OpenGL - Ethan Griffin");

	// Glew
	if (glewInit() != GLEW_OK)
	{
		// If Fail
		std::cout << "Glew Initialization failed" << std::endl;
		system("pause");
	}

	// Colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	program = ShaderLoader::CreateProgram("resources/shaders/vertexshader.txt", "resources/shaders/fragmentshader.txt");
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// mouse cursor
	//glutSetCursor(GLUT_CURSOR_NONE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Callbacks
	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	glutPassiveMotionFunc(mousePassiveInput);
	glutMotionFunc(mousePassiveInput);
	glutMouseFunc(MouseClick);
	glutDisplayFunc(Render);
	glutIdleFunc(Update);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
}



void Update()
{
	// Update game information	
	float currentTime = (float)glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (currentTime - previousTimeStamp) * 0.001f;
	previousTimeStamp = currentTime;

	if (deltaTime > (1.0f / 20.0f))
	{
		boid::instance().deleteBoid(false);
	}

	boid::instance().update(deltaTime);

	if (MouseState[0] == INPUT_DOWN_FIRST)
	{
		if (deltaTime < (1.0f / 20.0f))
		{
			boid::instance().newBoid();
		}	
				
		//MouseState[0] = INPUT_DOWN;
	}
	if (MouseState[2] == INPUT_DOWN_FIRST)
	{
		boid::instance().deleteBoid(false);
		//MouseState[2] = INPUT_DOWN;
	}
	if (KeyState['q'] == INPUT_DOWN_FIRST || KeyState['Q'] == INPUT_DOWN_FIRST)
	{
		boid::instance().deleteBoid(true);
		KeyState['q'] = INPUT_DOWN;
		KeyState['Q'] = INPUT_DOWN;
	}
	if (KeyState['c'] == INPUT_DOWN_FIRST || KeyState['C'] == INPUT_DOWN_FIRST)
	{
		if (boid::instance().m_bContain == true)
		{
			boid::instance().m_bContain = false;
		}
		else
		{
			boid::instance().m_bContain = true;
		}		
		KeyState['c'] = INPUT_DOWN;
		KeyState['C'] = INPUT_DOWN;
	}
	if (KeyState['f'] == INPUT_DOWN_FIRST || KeyState['F'] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(false);
		boid::instance().m_iMode = 11;
		KeyState['f'] = INPUT_DOWN;
		KeyState['F'] = INPUT_DOWN;
	}
	if (KeyState['l'] == INPUT_DOWN_FIRST || KeyState['L'] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(true);
		boid::instance().m_iMode = 10;
		KeyState['l'] = INPUT_DOWN;
		KeyState['L'] = INPUT_DOWN;
	}
	if (KeyState[48] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(false);
		boid::instance().m_iMode = 0;
		KeyState[48] = INPUT_DOWN;
	}
	if (KeyState[49] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(false);
		boid::instance().m_iMode = 1;
		KeyState[49] = INPUT_DOWN;
	}
	if (KeyState[50] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(true);
		boid::instance().m_iMode = 2;
		KeyState[50] = INPUT_DOWN;
	}
	if (KeyState[51] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(true);
		boid::instance().m_iMode = 3;
		KeyState[51] = INPUT_DOWN;
	}
	if (KeyState[52] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(false);
		boid::instance().m_iMode = 4;
		KeyState[52] = INPUT_DOWN;
	}
	if (KeyState[53] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(false);
		boid::instance().m_iMode = 5;
		KeyState[53] = INPUT_DOWN;
	}
	if (KeyState[54] == INPUT_DOWN_FIRST)
	{
		boid::instance().togglePlayerBoid(false);
		boid::instance().m_iMode = 6;
		KeyState[54] = INPUT_DOWN;
	}

	glutPostRedisplay();
}

 void mousePassiveInput(int x, int y)
{
	 boid::instance().m_mousePosition.x = x - (float)SCR_WIDTH/2;
	 boid::instance().m_mousePosition.y = -y + (float)SCR_HEIGHT/2;
}

void MouseClick(int button, int state, int x, int y)
{
	boid::instance().m_mousePosition.x = x - (float)SCR_WIDTH / 2;
	boid::instance().m_mousePosition.y = -y + (float)SCR_HEIGHT / 2;
	if (button >= 3)
		return;

	MouseState[button] = (state == GLUT_DOWN) ? INPUT_DOWN_FIRST : INPUT_UP_FIRST;
}

void KeyboardDown(unsigned char key, int x, int y)
{
	if (key > 96 && key < 123)
	{
		KeyState[key] = INPUT_DOWN_FIRST;
		KeyState[key - 32] = INPUT_DOWN_FIRST;
	}
	else if (key > 64 && key < 90)
	{
		KeyState[key] = INPUT_DOWN_FIRST;
		KeyState[key + 32] = INPUT_DOWN_FIRST;
	}
	else
	{
		KeyState[key] = INPUT_DOWN_FIRST;
	}
}

void KeyboardUp(unsigned char key, int x, int y)
{
	if (key > 96 && key < 123)
	{
		KeyState[key] = INPUT_UP_FIRST;
		KeyState[key - 32] = INPUT_UP_FIRST;
	}
	else if (key > 64 && key < 90)
	{
		KeyState[key] = INPUT_UP_FIRST;
		KeyState[key + 32] = INPUT_UP_FIRST;
	}
	else
	{
		KeyState[key] = INPUT_UP_FIRST;
	}
}