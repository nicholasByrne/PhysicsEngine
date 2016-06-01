#include "PhysicsObject.h"
#include "glm\ext.hpp"
#include "Gizmos.h"
#include <cmath>


DIYRigidBody::DIYRigidBody(glm::vec3 _position, glm::vec3 _velocity, glm::quat _rotation, float _mass, bool isStatic = false)
{
	m_position = _position;
	m_velocity = _velocity;
	//roation2D = _rotation;
	SetMass(_mass);
	SetStaticValue(isStatic);
}

void DIYRigidBody::Update(glm::vec3 gravity, float timeStep)
{
	if (!GetStaticValue())
	{
		m_acceleration += gravity;
		m_velocity += m_acceleration * timeStep;
		m_position += m_velocity * timeStep;
	}
	
	m_acceleration = glm::vec3(0);
}

void DIYRigidBody::Debug()
{
}

void DIYRigidBody::ApplyForce(glm::vec3 force)
{
	m_acceleration += (force / GetMass());
}

void DIYRigidBody::ApplyForceToActor(DIYRigidBody * actor2, glm::vec3 force) //Applies force towards other actor
{
	actor2->ApplyForce(force);
	ApplyForce(-force);
}

void SphereClass::Update(glm::vec3 gravity, float timeStep)
{
	DIYRigidBody::Update(gravity, timeStep);
}

void SphereClass::Debug()
{
}

void SphereClass::MakeGizmo()
{
	//Gizmos::addSphere(m_position, m_radius, 8, 8, glm::vec4(1, 0, 0, 1));
	Gizmos::addSphereFilled(m_position, m_radius, 8, 8, glm::vec4(1, 1, 0, 1));
}

PlaneClass::PlaneClass(glm::vec3 normal, float DistanceFromOrigin)
{
	m_shapeID = Plane;
	SetStaticValue(true);
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

void PhysicsObject::SetStaticValue(bool value)
{
	m_static = value;
}

float PhysicsObject::GetMass()
{
	if (!m_static)
	{
		return m_mass;
	}
	else
		return 100000000.f;
}

void BoxClass::Update(glm::vec3 gravtiy, float timeStep)
{
	DIYRigidBody::Update(gravtiy, timeStep);
}

void BoxClass::Debug()
{
}

void BoxClass::MakeGizmo()
{
	//Gizmos::addAABB(m_position, glm::vec3(m_length/2, m_height/2, m_width/2), glm::vec4(1, 1, 1, 1));
	Gizmos::addAABBFilled(m_position, glm::vec3(m_length / 2, m_height / 2, m_width / 2), glm::vec4(0, 0, 1, 1));
}

SpringJoint::SpringJoint(DIYRigidBody * connection1, DIYRigidBody * connection2, float springCoefficient, float damping)
{
	m_connections[0] = connection1;
	m_connections[1] = connection2;
	m_springCoefficient = springCoefficient;
	m_damping = damping;
	m_restLength = glm::length(m_connections[0]->m_position - m_connections[1]->m_position);
	m_shapeID = Joint;
}

void SpringJoint::Update(glm::vec3 gravity, float timeStep)
{
	glm::vec3 displacement = m_connections[0]->m_position - m_connections[1]->m_position; // From 1 to 0
	glm::vec3 positionDirection = glm::normalize(displacement);
	glm::vec3 restingDisplacement = positionDirection * m_restLength;
	glm::vec3 dampingVector = m_damping * (m_connections[1]->m_velocity - m_connections[0]->m_velocity);
	//m_connections[1]->ApplyForce(-m_springCoefficient * (restingDisplacement - displacement));// -dampingVector);
	m_connections[1]->ApplyForce(-m_springCoefficient * (restingDisplacement - displacement) - dampingVector);
	//m_connections[1]->ApplyForceToActor(m_connections[0], 0.5f * (-m_springCoefficient * (restingDisplacement - displacement) - dampingVector));
}

void SpringJoint::Debug()
{
}

void SpringJoint::MakeGizmo()
{
	Gizmos::addLine(m_connections[0]->m_position, m_connections[1]->m_position, glm::vec4(1, 1, 1, 1));
}
