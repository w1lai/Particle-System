#pragma once
#include "Particle.h"
class ParticleSystem
{
public:
	ParticleSystem(int startSize);
	~ParticleSystem();
	const int MAX_SIZE = 10000;
	
	bool collisionCheck;

	void populate(int size, glm::vec3 pos, glm::vec3 velo, glm::vec3 vPos, glm::vec3 vVelo, float life, float vLife, float radius);
	void update(
		//particle system parameters
		int subdivision, int regenRate, float density, float drag, float damper, float friction, bool collision, 
		//particle parameters
		glm::vec3 pos, glm::vec3 velo, glm::vec3 vPos, glm::vec3 vVelo, float life, float vLife, float radius,
		//aerodynamic force
		glm::vec3 windForce);
	void collideCorrection();
	void render(Camera* cam);
	void addForce(glm::vec3 Gforce, glm::vec3 Aforce);
private:
	int subdivision;
	int startingSize;
	int freshParticleSize; //debug variable
	float floorDamper;
	float floorFriction;

	float density;
	float dragCoefficient;
	std::vector<Particle> particles;


};

