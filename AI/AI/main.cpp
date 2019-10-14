
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
#include "GameManager.h"

#include "utils.h"

using namespace std;

GLuint programAnim;
GLuint program;

float deltaTime;
float previousTimeStamp;

void Render();
void Init(int argc, char **argv);
void Update();

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

	gameManager::instance().Draw();
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

	program = ShaderLoader::CreateProgram("resources/shaders/vertexshader2D.txt", "resources/shaders/fragmentshader2D.txt");
	/*
	programAnim = ShaderLoader::CreateProgram("resources/shaders/vertexshaderAnim.txt",
		"resources/shaders/fragmentshaderAnim.txt");*/

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// mouse cursor
	//glutSetCursor(GLUT_CURSOR_NONE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Callbacks
	glutKeyboardFunc(inputGlut::instance().KeyboardDown);
	glutKeyboardUpFunc(inputGlut::instance().KeyboardUp);
	glutSpecialFunc(inputGlut::instance().specialKeyboardDown);
	glutSpecialUpFunc(inputGlut::instance().specialKeyboardUp);
	glutPassiveMotionFunc(inputGlut::instance().mousePassiveInput);
	glutMotionFunc(inputGlut::instance().MouseClick);
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

	gameManager::instance().Update(deltaTime);

	glutPostRedisplay();
}

