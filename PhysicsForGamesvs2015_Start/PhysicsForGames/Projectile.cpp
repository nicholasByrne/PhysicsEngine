#include "Projectile.h"





Projectile::Projectile(SphereClass* sphereclass, float angle, float speed)
{
	m_rigidBody = sphereclass;
	m_angle = angle;
	m_speed = speed;
	m_startPosition = sphereclass->m_position;
	m_initialVelocity = sphereclass->m_velocity;
	m_gravity = glm::vec3(0);
}

Projectile::~Projectile()
{
}

void Projectile::Update(glm::vec3 gravity, float fDeltaTime)
{
	m_gravity = gravity;
}

void Projectile::Draw()
{
	//Draw timesteps
	for (float timeStep = 0; timeStep < 5; timeStep += 0.2)
	{
		m_rigidBody->m_position.x = m_startPosition.x + m_speed * timeStep * glm::cos(m_angle);
		m_rigidBody->m_position.y = m_startPosition.y + m_speed * timeStep * glm::sin(m_angle) - 0.5f * m_gravity.y * (timeStep * timeStep);
	}
}
