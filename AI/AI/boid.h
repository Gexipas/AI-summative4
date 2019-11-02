#pragma once

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>    
#include <time.h> 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ShaderLoader.h"
#include "camera.h"

using namespace glm;

struct Vertex
{
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
};

class boid
{
public:
	bool m_boidSpawn;
	vec2 m_mousePosition;
	std::vector<vec2> m_velocityBoids;
	std::vector<vec2> m_positionBoids;
	std::vector<vec3> m_colorBoids;
	vec2 playerVelocityBoid;
	vec2 playerPositionBoid;
	float m_maxSpeed = 300.0f;
	float m_maxforce = 10.0f;
	float detectRadius = 80.0f;
	float wanderRadius = 60.0f;
	float maxBoidNumber = 5000.0f;
	float pathRadius = 20.0f;
	bool m_bContain = false;
	int m_iMode = 0;

	static boid& instance()
	{
		static boid *instance = new boid();
		return *instance;
	}

	void update(float _deltatime);
	void render(GLuint program);
	
	void newBoid();// create Boid	
	void deleteBoid(bool _bDelAll);// delete Boid
	void togglePlayerBoid(bool _bOn);// toggle player boid

private:
	// variables	
	unsigned int VBO, EBO;
	unsigned int VAO;
	unsigned int texture1;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// singleton
	boid()
	{
		setup2DMesh();
		togglePlayerBoid(false);
		srand((unsigned int)time(NULL));
	}

	//added behaviours
	void arriveControlled(float _deltatime);// player controlled boid

	//simple behaviours
	void seek(int boid, float _deltatime, vec2 _endPosition);// seek function
	vec2 flee(int _boid);// flee function
	vec2 pursuit(int _boid);// pursuit function
	vec2 evade(int _boid);// evade function
	void arrive(int boid, float _deltatime, vec2 _endPosition);// arrive function
	vec2 wandering(int _boid);// wander function
	void containment(int boid, bool bContain);// containment function
	vec2 pathFollowing(int _boid);// path following

	// advanced behaviours
	vec2 flocking(int _boid);// flocking function
	vec2 leader(int _boid);// leader following function
	
	// texture
	void setup2DMesh();// setup 2d mesh	
	unsigned int loadTexture(char const * path);// load texture for boid

};