#pragma once

#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "boidMesh.h"
#include "camera.h"

using namespace glm;

class boid
{
public:
	vec2 m_velocity;
	vec2 m_position;

	boid(vec2 _velocity, vec2 _position)
	{
		m_velocity = _velocity;
		m_position = _position;
	}

	void update(float _deltatime)
	{

	}

	void render(GLuint program)
	{
		float angle = atan2(m_velocity.y, m_velocity.x);
		glm::mat4 pv = Camera::instance().cameraMatrix();
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3(m_position,0.0f));
		glm::mat4 rotationZ = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 model = translationMatrix * rotationZ;
		glm::mat4 pvm = pv * model;
		glUniformMatrix4fv(glGetUniformLocation(program, "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
		Mesh::instance().Draw(program);
	}

private:

};