#include "PhysicsObject.h"
#include "glm\ext.hpp"
#include "Gizmos.h"



DIYRigidBody::DIYRigidBody(glm::vec3 _position, glm::vec3 _velocity, glm::quat _rotation, float _mass)
{
	m_position = _position;
	m_velocity = _velocity;
	//roation2D = _rotation;
	m_mass = _mass;
}

void DIYRigidBody::Update(glm::vec3 gravity, float timeStep)
{
	m_velocity += gravity * timeStep;
	m_position += m_velocity * timeStep;
	
	m_acceleration = glm::vec3(0);
}

void DIYRigidBody::Debug()
{
}

void DIYRigidBody::ApplyForce(glm::vec3 force)
{
	m_acceleration += (force / m_mass);
}

void DIYRigidBody::ApplyForceToActor(DIYRigidBody * actor2, glm::vec3 force)
{
	actor2->ApplyForce(force);
}

//SphereClass::SphereClass(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour)
//{
//
//}

void SphereClass::MakeGizmo()
{
	//Gizmos::addSphere(m_position, m_radius, 8, 8, glm::vec4(1, 0, 0, 1));
	Gizmos::addSphereFilled(m_position, m_radius, 8, 8, glm::vec4(1, 1, 0, 1));
}
