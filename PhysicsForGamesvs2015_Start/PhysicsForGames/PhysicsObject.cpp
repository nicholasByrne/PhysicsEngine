#include "PhysicsObject.h"
#include "glm\ext.hpp"
#include "Gizmos.h"
#include <cmath>


DIYRigidBody::DIYRigidBody(glm::vec3 _position, glm::vec3 _velocity, glm::quat _rotation, float _mass)
{
	m_position = _position;
	m_velocity = _velocity;
	//roation2D = _rotation;
	m_mass = _mass;
}

void DIYRigidBody::Update(glm::vec3 gravity, float timeStep)
{
	//m_velocity += gravity * timeStep;
	m_acceleration += gravity * timeStep;
	m_velocity += m_acceleration;
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

void DIYRigidBody::ApplyForceToActor(DIYRigidBody * actor2, glm::vec3 force) //Applies force towards other actor
{
	actor2->ApplyForce(force);
	ApplyForce(-force);
}

void SphereClass::MakeGizmo()
{
	//Gizmos::addSphere(m_position, m_radius, 8, 8, glm::vec4(1, 0, 0, 1));
	Gizmos::addSphereFilled(m_position, m_radius, 8, 8, glm::vec4(1, 1, 0, 1));
}

PlaneClass::PlaneClass(glm::vec3 normal, float DistanceFromOrigin)
{
	m_shapeID = Plane;
	m_normal = normal;
	m_originDist = DistanceFromOrigin;
}

void PlaneClass::Update(glm::vec3 gravity, float timeStep)
{

}

void PlaneClass::Debug()
{

}

void PlaneClass::MakeGizmo()
{
	glm::vec3 perpNorm = glm::vec3(m_normal.z, m_normal.y, -m_normal.x);
	glm::vec3 perpNorm2 = glm::cross(m_normal, perpNorm);


	glm::vec3 centrePoint = m_normal * m_originDist;

	glm::mat4 t(1);
	t[0] = glm::vec4(perpNorm,0);
	t[1] = glm::vec4(m_normal,0);
	t[2] = glm::vec4(perpNorm2, 0);


	//glm::vec3 parallel = glm::normalize(perpNorm);// ???
	//glm::mat4 transform = glm::lookAt(glm::vec3(), perpNorm2, m_normal);
	glm::vec4 colour(1, 1, 1, 1);

	Gizmos::addDisk(centrePoint, 10, 4, colour, &t);
	Gizmos::addTransform(glm::translate(centrePoint) * t);
	//Gizmos::add2Line(start.xy(), end.xy(), colour);
}
