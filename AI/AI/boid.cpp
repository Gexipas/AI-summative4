
#include "boid.h"

void boid::update(float _deltatime)
{
	if (playerPositionBoid.x != 800.0f)
	{
		arriveControlled(_deltatime);
	}
	switch (m_iMode)
	{
	case 0:
	{
		togglePlayerBoid(false);
		for (unsigned int i = 0; i < m_velocityBoids.size(); i++)
		{
			seek(i, _deltatime, m_mousePosition);
		}
		break;
	}
	case 1:
	{
		togglePlayerBoid(false);
		for (unsigned int i = 0; i < m_velocityBoids.size(); i++)
		{
			seek(i, _deltatime, flee(i));
		}
		break;
	}
	case 2:
	{
		for (unsigned int i = 0; i < m_velocityBoids.size(); i++)
		{
			seek(i, _deltatime, pursuit(i));
		}
		break;
	}
	default:
	{
		break;
	}
	}
	
}

void boid::render(GLuint program)
{
	glm::mat4 pv = Camera::instance().cameraMatrix();
	   	
	for (unsigned int i = 0; i < m_velocityBoids.size(); i++)
	{
		glUseProgram(program);

		// color
		glm::vec3 color = m_colorBoids[i];
		glUniform3f(glGetUniformLocation(program, "colorBoid"), color.x, color.y, color.z);

		float angle = -0.785398f + atan2(m_velocityBoids[i].y, m_velocityBoids[i].x);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3(m_positionBoids[i], -maxBoidNumber + (float)i));
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
	if (playerPositionBoid.x != 800.0f)
	{
		glUseProgram(program);

		// color
		glm::vec3 color = { 1.0f,1.0f,1.0f };
		glUniform3f(glGetUniformLocation(program, "colorBoid"), color.x, color.y, color.z);

		float angle = -0.785398f + atan2(playerVelocityBoid.y, playerVelocityBoid.x);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3(playerPositionBoid, 1.0f));
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

void boid::newBoid()
{
	if (m_velocityBoids.size() < (unsigned int)maxBoidNumber)
	{
		m_velocityBoids.push_back({ 10.0f,10.0f });
		m_positionBoids.push_back(m_mousePosition);
		float R = (float)(rand() % 10001) / 10000;
		float G = (float)(rand() % 10001) / 10000;
		float B = (float)(rand() % 10001) / 10000;
		m_colorBoids.push_back({ R,G,B });
	}
}

void boid::deleteBoid(bool _bDelAll)
{
	if (_bDelAll == true)
	{
		m_velocityBoids.clear();
		m_positionBoids.clear();
		m_colorBoids.clear();
	}
	else
	{
		if (m_velocityBoids.size() > 0)
		{
			m_velocityBoids.pop_back();
			m_positionBoids.pop_back();
			m_colorBoids.pop_back();
		}
	}
}

void boid::togglePlayerBoid(bool _bOn)
{
	if (_bOn == true)
	{
		playerVelocityBoid = { 10.0f,10.0f };
		playerPositionBoid = m_mousePosition;
	}
	else
	{
		playerVelocityBoid = { 0.0f,0.0f };
		playerPositionBoid = { 800.0f,0.0f };
	}
	
}

void boid::arriveControlled(float _deltatime)
{
	vec2 position = playerPositionBoid;
	vec2 velocity = playerVelocityBoid;
	vec2 accel;
	vec2 desiredRoute = m_mousePosition - position;

	// limit desiredRoute
	if (!(desiredRoute.x == 0 && desiredRoute.y == 0))
	{
		if (glm::length(desiredRoute) < 100.0f)// slow for arrival
		{
			desiredRoute = normalize(desiredRoute)*m_maxSpeed*(glm::length(desiredRoute) / 100.0f);
		}
		else
		{
			desiredRoute = normalize(desiredRoute)*m_maxSpeed;
		}
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
	playerVelocityBoid = newVelocity;

	//pos = pos + v *delta
	playerPositionBoid = position + (newVelocity * _deltatime);
	if (playerVelocityBoid.x > (float)SCR_WIDTH / 2) // right edge screen collision
	{
		playerVelocityBoid.x = -((float)SCR_WIDTH / 2) + 1.0f;
	}
	if (playerVelocityBoid.x < -(float)SCR_WIDTH / 2) // left edge screen collision
	{
		playerVelocityBoid.x = ((float)SCR_WIDTH / 2) - 1.0f;
	}
	if (playerVelocityBoid.y > (float)SCR_HEIGHT / 2) // top edge screen collision
	{
		playerVelocityBoid.y = -((float)SCR_HEIGHT / 2) + 1.0f;
	}
	if (playerVelocityBoid.y < -(float)SCR_HEIGHT / 2) // bottom edge screen collision
	{
		playerVelocityBoid.y = ((float)SCR_HEIGHT / 2) - 1.0f;
	}
}

void boid::seek(int boid, float _deltatime, vec2 _endPosition)
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
	containment(boid, m_bContain);
}

vec2 boid::flee(int _boid)
{
	vec2 mouse = m_mousePosition - m_positionBoids[_boid];
	return (mouse*-1.0f) + m_positionBoids[_boid];
}

vec2 boid::pursuit(int _boid)
{
	vec2 targetPos = playerPositionBoid;
	vec2 targetDir = normalize(playerVelocityBoid);
	float distance = glm::length(m_positionBoids[_boid] - targetPos);
	vec2 targetLoc = targetPos + (targetDir*distance*0.9f);
	return (targetLoc);
}

vec2 boid::evade(int _boid)
{
	vec2 targetPos = playerPositionBoid;
	vec2 targetDir = normalize(playerVelocityBoid);
	float distance = glm::length(m_positionBoids[_boid] - targetPos);
	vec2 targetLoc = targetPos - (targetDir*distance*1.1f);
	return (targetLoc);
}

void boid::arrive(int boid, float _deltatime, vec2 _endPosition)
{
	vec2 position = m_positionBoids[boid];
	vec2 velocity = m_velocityBoids[boid];
	vec2 accel;
	vec2 desiredRoute = _endPosition - position;

	// limit desiredRoute
	if (!(desiredRoute.x == 0 && desiredRoute.y == 0))
	{
		if (glm::length(desiredRoute) < 100.0f)// slow for arrival
		{
			desiredRoute = normalize(desiredRoute)*m_maxSpeed*(glm::length(desiredRoute) / 100.0f);
		}
		else
		{
			desiredRoute = normalize(desiredRoute)*m_maxSpeed;
		}
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
	containment(boid, m_bContain);
}

vec2 boid::wandering(int _boid)
{
	vec2 forward = normalize(m_velocityBoids[_boid]) * (wanderRadius*2.0f);
	float randAngle = radians((float)(rand() % 360 + 1));

	vec2 randVec = { wanderRadius*cosf(randAngle), wanderRadius*sinf(randAngle) };
	return(forward + randVec + m_positionBoids[_boid]);
}

void boid::containment(int boid, bool bContain)
{
	if (bContain == true)
	{
		if (m_positionBoids[boid].x > (float)SCR_WIDTH / 2) // right edge screen collision
		{
			m_velocityBoids[boid].x = -m_velocityBoids[boid].x;
		}
		if (m_positionBoids[boid].x < -(float)SCR_WIDTH / 2) // left edge screen collision
		{
			m_velocityBoids[boid].x = -m_velocityBoids[boid].x;
		}
		if (m_positionBoids[boid].y > (float)SCR_HEIGHT / 2) // top edge screen collision
		{
			m_velocityBoids[boid].y = -m_velocityBoids[boid].y;
		}
		if (m_positionBoids[boid].y < -(float)SCR_HEIGHT / 2) // bottom edge screen collision
		{
			m_velocityBoids[boid].y = -m_velocityBoids[boid].y;
		}
	}
	else
	{
		if (m_positionBoids[boid].x > (float)SCR_WIDTH / 2) // right edge screen collision
		{
			m_positionBoids[boid].x = -((float)SCR_WIDTH / 2) + 1.0f;
		}
		if (m_positionBoids[boid].x < -(float)SCR_WIDTH / 2) // left edge screen collision
		{
			m_positionBoids[boid].x = ((float)SCR_WIDTH / 2) - 1.0f;
		}
		if (m_positionBoids[boid].y > (float)SCR_HEIGHT / 2) // top edge screen collision
		{
			m_positionBoids[boid].y = -((float)SCR_HEIGHT / 2) + 1.0f;
		}
		if (m_positionBoids[boid].y < -(float)SCR_HEIGHT / 2) // bottom edge screen collision
		{
			m_positionBoids[boid].y = ((float)SCR_HEIGHT / 2) - 1.0f;
		}
	}
}

vec2 boid::pathFollowing(int _boid)
{
	vec2 start;
	vec2 end;
	if (m_positionBoids[_boid].x > -(float)(SCR_WIDTH / 2) && m_positionBoids[_boid].x <= -200)
	{
		start = { -(float)(SCR_WIDTH / 2),0.0f };
		end = { -200,300.0f };
	}
	else if (m_positionBoids[_boid].x > -200 && m_positionBoids[_boid].x <= 200)
	{
		start = { -200,300.0f };
		end = { 200,-50.0f };
	}
	else
	{
		start = { 200,-50.0f };
		end = { (float)(SCR_WIDTH / 2),0.0f };
	}

	// p = v norm * speed + pos
	vec2 point = normalize(m_velocityBoids[_boid]) * 200.0f + m_positionBoids[_boid];


	// a = p - start
	vec2 a = point - start;
	// b = end - start
	vec2 b = end - start;
	// a . (b/|b|)
	float distance = dot(a, (b/length(b)));
	// r = b norm * distance
	vec2 result = normalize(b) * distance;
	return (result);
}

vec2 boid::flocking(int _boid)
{
	// mouse follow
	vec2 mouse = m_mousePosition - m_positionBoids[_boid];

	// variables
	vec2 seperation = { 0.0f,0.0f };
	vec2 allignment = { 0.0f,0.0f };
	vec2 cohesion = { 0.0f,0.0f };

	// check if more than one
	if (m_velocityBoids.size() > 1)
	{
		//seperation
		for (unsigned int j = 0; j < m_velocityBoids.size(); j++)
		{
			if (j != _boid && glm::length(m_positionBoids[_boid] - m_positionBoids[j]) < detectRadius / 4.0f)
			{
				vec2 vector = m_positionBoids[_boid] - m_positionBoids[j];
				seperation = seperation + vector;
			}
		}
		seperation = seperation / (float)(m_velocityBoids.size() - 1);

		// allignment			
		for (unsigned int j = 0; j < m_velocityBoids.size(); j++)
		{
			if (j != _boid && glm::length(m_positionBoids[_boid] - m_positionBoids[j]) < detectRadius)
			{
				allignment = allignment + m_velocityBoids[j];
			}
		}
		allignment = allignment / (float)(m_velocityBoids.size() - 1);

		//cohesion			
		for (unsigned int j = 0; j < m_velocityBoids.size(); j++)
		{
			if (j != _boid && glm::length(m_positionBoids[_boid] - m_positionBoids[j]) < detectRadius)
			{
				cohesion = cohesion + m_positionBoids[j];
			}
		}
		cohesion = cohesion / (float)(m_velocityBoids.size() - 1);
		vec2 cohesionVec = cohesion - m_positionBoids[_boid];
	}

	// flocking
	vec2 total = ((seperation * 1000.0f) + (allignment*10.0f) + (cohesion * 2.0f) + mouse) / 4.0f;
	return(total + m_positionBoids[_boid]);
}

void boid::setup2DMesh()
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

unsigned int boid::loadTexture(char const * path)
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