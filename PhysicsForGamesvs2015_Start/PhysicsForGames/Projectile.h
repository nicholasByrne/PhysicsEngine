#pragma once
#include "PhysicsObject.h"

class Projectile
{
public:
	Projectile(SphereClass* sphereclass, float angle, float speed);
	~Projectile();

	void Update(glm::vec3 gravity, float fDeltaTime);
	void Draw();

	//glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour)
	SphereClass* m_rigidBody;
	glm::vec3 m_startPosition;
	glm::vec3 m_initialVelocity;
	//glm::vec3 m_currentPosition;

	glm::vec3 m_gravity;

	float m_angle;
	float m_speed;
};

