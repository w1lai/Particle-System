
#include "Particle.h"
#include "core.h"
#include <random>

extern int FLOOR;

//debugging particle class
Particle::Particle() 
{
	radius = 0.05;
	mass = 0.1;
	position = glm::vec3(0);
	velocity = glm::vec3(0.0f, 5.0f, 0.0f);
	varPosition = glm::vec3(0);
	varVelocity = glm::vec3(0);
	force = glm::vec3(0);
	lifespan = 1;
	currentLife = 1;
	worldMatrix = glm::mat4(1);
	geometry = new Cube(glm::vec3(-radius), glm::vec3(radius));

	collisionCheck = true;
	
	pertributeLife(0.9);
	perturbateVector(&position, 0.3);
	perturbateVector(&velocity, 3);

}


Particle::Particle(float m, glm::vec3 p, glm::vec3 v, glm::vec3 vP, glm::vec3 vV, float r, float l, float vL, bool c) 
{

	radius = r;
	mass = m;
	position = p;
	velocity = v;
	varPosition = vP;
	varVelocity = vV;
	force = glm::vec3(0);
	lifespan = l;
	currentLife = l;
	worldMatrix = glm::mat4(1);
	geometry = new Cube(glm::vec3(-radius), glm::vec3(radius));

	collisionCheck = true;

	pertributeLife(vL);
	perturbateVector(&position, varPosition);
	perturbateVector(&velocity, varVelocity);

}

void Particle::perturbateVector(glm::vec3* v, float variance)
{
	
	for (int i = 0; i < 6; i++)
	{
		int pm = (rand() % 2 == 0) ? 1 : -1;
		float amount = variance * (float)rand() / (RAND_MAX);
		switch (i)
		{
		case 0:
			v->x += pm * amount;
			break;

		case 1:
			v->y += pm * amount;
			break;

		case 2:
			v->z += pm * amount;
			break;

		default:
			break;
		}
	}
}

void Particle::perturbateVector(glm::vec3* v, glm::vec3 var)
{
	int pm = (rand() % 2 == 0) ? 1 : -1;
	float a = var.x * (float)rand() / (RAND_MAX);
	v->x += pm * a;

	pm = (rand() % 2 == 0) ? 1 : -1;
	a = var.y * (float)rand() / (RAND_MAX);
	v->y += pm * a;

	pm = (rand() % 2 == 0) ? 1 : -1;
	a = var.z * (float)rand() / (RAND_MAX);
	v->z += pm * a;
	
}

void Particle::pertributeLife(float variance)
{

	int pm = (rand() % 2 == 0) ? 1 : -1;
	float amount = variance * (float)rand() / (RAND_MAX);
	currentLife += pm * amount;

	//just in case
	if (currentLife <= 0)
	{
		currentLife = 0.01f;
	}

}

void Particle::applyForce(glm::vec3 f)
{
	force += f;
}

void Particle::integrate(float dT)
{
	//update currentLife
	currentLife -= dT;

	//semi-forward euler integration
	glm::vec3 acceleration = (1 / mass) * force;
	velocity += acceleration * dT;
	position += velocity * dT;
	//reset force on the particle
	force = glm::vec3(0);

	//update position to geometry
	worldMatrix = glm::translate(glm::mat4(1), position);
}

//floor detection
void Particle::collisionCorrection(float damper, float friction)
{

	//collisionCheck is to avoid particles that is well below ground gets bounced up when enabling collision on floor

	if (collisionCheck && position.y < FLOOR)
	{
		position = glm::vec3(position.x, FLOOR, position.z);
		//apply friction
		float magnitude = mass * 9.8f;
		applyForce(-magnitude * friction * glm::normalize(glm::vec3(velocity.x, 0.0f, velocity.z)));

		velocity = glm::vec3(damper * velocity.x, damper * -velocity.y, damper* velocity.z);
		


	}
}

bool Particle::isAlive()
{
	return currentLife > 0;
}

void Particle::draw(Camera* c)
{
	//draw the particle if it's alive (avoid swap schenanigans that has a nullptr geometry)

	if (!isAlive())
	{
		return;
	}

	//draw the particle

	geometry->draw(c->GetViewProjectMtx() * worldMatrix, Window::shaderProgram);
}


//getters
float Particle::getMass()
{
	return mass;
}

//calculating drag force
glm::vec3 Particle::calcDrag(glm::vec3 AeroForce, float density, float dragCoefficient)
{
	float PI = 3.14159;
	//TODO implement drag force formula
	glm::vec3 relativeVelo = velocity - AeroForce;
	glm::vec3 dir = glm::normalize(relativeVelo);

	float area = PI * this->radius * this->radius;

	glm::vec3 result = 0.5f * density * glm::length(relativeVelo) * glm::length(relativeVelo) * dragCoefficient * area * dir;
	return result;
}