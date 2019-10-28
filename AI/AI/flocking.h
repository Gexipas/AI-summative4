#pragma once


//separation
//check local objects - 100px radius
//average vectors from object to self
//seek - desired velocity

//allignment
//check local objects - 100px radius
//average object vectors
//seek - desired velocity

//cohesion
//check local objects - 100px radius
//average position of objects
//seek to average position

//average the average velocity
//seek

//seperation
int self;
vec2 position;
vec2 total = { 0.0f,0.0f }
for (unsigned int i = 0; i < vec; i++)
{
	if (i != self)
	{
		vec2 vector = 1 - 2;
		total = total + vec;
	}
}
total = total / (vecsize-1);

// allignment
int self;
vec2 position;
vec2 total = {0.0f,0.0f}
for (unsigned int i = 0; i < vec; i++)
{
	if (i != self)
	{
		total = total + vec;
	}
}
total = total / (vecsize-1);

//cohesion
vec2 total = { 0.0f,0.0f }
for (unsigned int i = 0; i < vec; i++)
{
	if (i != self)
	{
		total = total + vec;
	}
}
total = total / (vecsize-1);