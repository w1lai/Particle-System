#include <iostream>
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(int startSize)
{
	srand(time(0));
	startingSize = (startSize >= MAX_SIZE)? MAX_SIZE: startSize;
	subdivision = 60;
	freshParticleSize = startingSize;
	floorDamper = 0.9;
	floorFriction = 0.0f;

	density = 8;
	dragCoefficient = 0.4;
	collisionCheck = true;
}

void ParticleSystem::populate(int amount, glm::vec3 pos, glm::vec3 velo, glm::vec3 vPos, glm::vec3 vVelo, float life, float vLife, float radius)
{
	static float mass = 0.1;
	for (int i = 0; i < amount; i++)
	{
		//make a particle
		Particle p(mass, pos, velo, vPos, vVelo, radius, life, vLife, collisionCheck);
		//Particle p;
		//push it into stack
		particles.push_back(p);
	}

}

void ParticleSystem::addForce(glm::vec3 gravityForce, glm::vec3 aeroDynamicForce)
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].applyForce(gravityForce);
		particles[i].applyForce(aeroDynamicForce);
	}
}

void ParticleSystem::update(int subdivision, int particleRegenRate, float density, float drag, float damper, float friction, bool collision,
	glm::vec3 pos, glm::vec3 velo, glm::vec3 vPos, glm::vec3 vVelo, float life, float vLife, float radius, glm::vec3 aeroForce)
{

	this->density = density;
	this->dragCoefficient = drag;
	this->floorDamper = damper;
	this->floorFriction = friction;
	this->collisionCheck = collision;
	// remove all particles that is not alive
	for (int i = 0; i < particles.size(); i++)
	{
		if (!particles[i].isAlive())
		{
			//delete the geometry on this particle
			delete particles[i].geometry;

			//copy the last particle to this one, size - 1
			particles[i] = particles.back();
			particles.pop_back();
		}
	}



	//populate with new particles
	int actualPopulate = ((MAX_SIZE - particles.size()) > particleRegenRate)? particleRegenRate: (MAX_SIZE - particles.size());
	freshParticleSize = actualPopulate;
	populate(actualPopulate, pos, velo, vPos, vVelo, life, vLife, radius);


	//update all particles currentLife
	float dT = (float)1 / (float)subdivision;
	
	//calculate new position
	for (int i = 0; i < particles.size(); i++)
	{
		float mass = particles[i].getMass();
		particles[i].applyForce(mass * glm::vec3(0.0f, -9.8f, 0.0f));
		
		glm::vec3 forceAero = aeroForce;
		glm::vec3 dragForce = particles[i].calcDrag(forceAero, density, dragCoefficient);

		particles[i].applyForce(forceAero);
		particles[i].applyForce(-dragForce);

		particles[i].integrate(dT);

	}

	//collide correction if applicable
	collideCorrection();
	

}

void ParticleSystem::render(Camera* cam)
{
	//display size for now
	//std::cout << "particle system size : " << particles.size() << ", new particle: " << freshParticleSize << std::endl;


	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].draw(cam);
	}

	//seems working?
}


void ParticleSystem::collideCorrection()
{
	
	if (!collisionCheck)
	{
		return;
	}
	
	
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].collisionCorrection(floorDamper, floorFriction);
	}
}


//destructor
ParticleSystem::~ParticleSystem()
{
	for (int i = 0; i < particles.size(); i++)
	{
		//just in case
		if (particles[i].geometry != nullptr)
		{
			delete particles[i].geometry;
		}
	}
}