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
	float m_maxSpeed = 100.0f;
	float m_maxforce = 1.0f;

	static boid& instance()
	{
		static boid *instance = new boid();
		return *instance;
	}

	void update(float _deltatime)
	{
		for (unsigned int i = 0; i < m_velocityBoids.size(); i++)
		{
			seek(i, _deltatime, m_mousePosition);
		}
	}

	void render(GLuint program)
	{
		glm::mat4 pv = Camera::instance().cameraMatrix();
		
		for (unsigned int i = 0; i < m_velocityBoids.size(); i++)
		{
			glUseProgram(program);

			// color
			glm::vec3 color = m_colorBoids[i];
			glUniform3f(glGetUniformLocation(program, "colorBoid"),color.x, color.y, color.z);

			float angle = -0.785398f + atan2(m_velocityBoids[i].y, m_velocityBoids[i].x);
			glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3(m_positionBoids[i], -100.0f +(float)i));
			glm::mat4 rotationZ = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 model = translationMatrix * rotationZ;
			glm::mat4 pvm = pv * model;
			glUniformMatrix4fv(glGetUniformLocation(program, "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(program, "tex"), 0);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			glUseProgram(0);
		}		
	}

	void newBoid()
	{
		m_velocityBoids.push_back({ 1.0f,0.0f });
		m_positionBoids.push_back(m_mousePosition);
		float R = (float)(rand() % 10001) / 10000;
		float G = (float)(rand() % 10001) / 10000;
		float B = (float)(rand() % 10001) / 10000;
		m_colorBoids.push_back({ R,G,B });
	}

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
		srand((unsigned int)time(NULL));
	}

	// seek function
	void seek(int boid, float _deltatime, vec2 _endPosition)
	{
		vec2 position = m_positionBoids[boid];
		vec2 velocity = m_velocityBoids[boid];
		vec2 accel;
		vec2 desiredRoute = _endPosition - position;

		// limit desiredRoute
		if (!(desiredRoute.x == 0 && desiredRoute.y == 0))
		{
			desiredRoute = normalize(desiredRoute)*m_maxSpeed;
		}
		vec2 force = desiredRoute - velocity;

		// limit force
		if (glm::length(force) > m_maxforce)
		{
			force = normalize(force)*m_maxforce;
		}

		//a = a+f
		accel = accel + force;

		//v = u+a
		vec2 newVelocity = velocity + accel;
		if (!(newVelocity.x == 0 && newVelocity.y == 0)) // cap speed
		{
			if (glm::length(newVelocity) > m_maxSpeed)
			{
				newVelocity = normalize(newVelocity)*m_maxSpeed;
			}
		}
		m_velocityBoids[boid] = newVelocity;

		//pos = pos + v *delta
		m_positionBoids[boid] = position + (newVelocity * _deltatime);
	}

	// setup 2d mesh
	void setup2DMesh()
	{
		std::string totalPath = "resources/texture/boid.png";
		char const * path = totalPath.c_str();

		glGenTextures(1, &texture1);

		int width, height;
		unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
			   
		if (image)
		{
			glBindTexture(GL_TEXTURE_2D, texture1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			SOIL_free_image_data(image);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			SOIL_free_image_data(image);
		}

		float center = 16.0f;

		float left = 0.0f - center;
		float right = (float)width - center;
		float top = 0.0f + center;
		float bot = (float)-height + center;

		vertices =
		{
			// Position                 // Normal                //TexCoords
			{{ left,  top,  0.0f},    {0.0f,  0.0f,  1.0f},    {0.0f, 0.0f}},
			{{ left,  bot,  0.0f},    {0.0f,  0.0f,  1.0f},    {0.0f, 1.0f}},
			{{ right, bot,  0.0f},    {0.0f,  0.0f,  1.0f},    {1.0f, 1.0f}},
			{{ right, top,  0.0f},    {0.0f,  0.0f,  1.0f},    {1.0f, 0.0f}}
		};

		indices =
		{
			0, 1, 2,
			0, 2, 3,
		};

		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}

	// load texture for boid
	unsigned int loadTexture(char const * path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height;
		unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
		if (image)
		{
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			SOIL_free_image_data(image);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			SOIL_free_image_data(image);
		}

		return textureID;
	}

};