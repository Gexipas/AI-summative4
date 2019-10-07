#pragma once

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

class seek
{
public:
	seek(int _iX, int _iY)
	{
		position = vec2(_iX, _iY);
	}
	~seek(){}

	void update(float deltatime, vec2 _endPosition)
	{
		endPosition = _endPosition;
		vec2 desiredRoute = endPosition - position;

		// limit desiredRoute
		if (!(desiredRoute.x == 0 && desiredRoute.y == 0))
		{
			if (glm::length(desiredRoute) > maxSpeed)
			{
				desiredRoute = normalize(desiredRoute)*maxSpeed;
			}
			vec2 force = desiredRoute - vectorMove;

			// limit force
			if (!(force.x == 0 && force.y == 0))
			{
				if (glm::length(force) > maxSpeed)
				{
					force = normalize(force)*maxSpeed;
				}

				//a =a+f
				//v = u+a
				//pos = pos + v *delta
			}						
		}
	}

	vec2 endPosition;
	vec2 position;

	vec2 vectorMove;
	float currentSpeed;
	float maxSpeed;


	float maxForce;
private:

};