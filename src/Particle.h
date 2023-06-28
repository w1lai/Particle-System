#pragma once
#include "Camera.h"
#include "Cube.h"
class Particle
{
public:

	Particle();
	Particle(float m, glm::vec3 p, glm::vec3 v, glm::vec3 vP, glm::vec3 vV, float r, float l, float vL, bool c);
	void applyForce(glm::vec3 f);
	void integrate(float dT);
	bool isAlive();
	void draw(Camera* c);

	void pertributeLife(float variance);
	void collisionCorrection(float damper, float friction);

	//getters
	float getMass();
	glm::vec3 calcDrag(glm::vec3 forceAero, float density, float dragCoefficient);

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;

	Cube* geometry;
	//variances for initial position and velocity


private:

	//debug function
	void perturbateVector(glm::vec3* v, float variance);
	void perturbateVector(glm::vec3* v, glm::vec3 var);

	glm::vec3 varPosition;
	glm::vec3 varVelocity;
	glm::mat4 worldMatrix;
	float lifespan;
	float currentLife;
	float varLife;
	float mass;
	float radius;
	bool collisionCheck;
};

