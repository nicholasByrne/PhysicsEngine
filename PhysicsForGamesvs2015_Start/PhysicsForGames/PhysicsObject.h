#pragma once
#include "glm\glm.hpp"
#include "glm\ext.hpp"

enum ShapeType
{
	Plane = 0,
	Sphere = 1,
	Box = 2,
};

class PhysicsObject
{
public:
	PhysicsObject() {};
	~PhysicsObject() {};

	ShapeType m_shapeID;
	void virtual Update(glm::vec3 gravity, float timeStep) = 0;
	void virtual Debug() = 0;
	void virtual MakeGizmo() = 0;
	void virtual ResetPosition() {};
};


class DIYRigidBody : public PhysicsObject
{
public:
	DIYRigidBody(glm::vec3 position, glm::vec3 velocity, glm::quat rotation, float mass);

	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;

	float m_mass;
	float m_roation2D; //2D rotation only needs a single float

	virtual void Update(glm::vec3 gravity, float timeStep);
	virtual void Debug();
	void ApplyForce(glm::vec3 force);
	void ApplyForceToActor(DIYRigidBody* actor2, glm::vec3 force);
};

class SphereClass : public DIYRigidBody
{
public: 
	float m_radius;
	SphereClass(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour) : DIYRigidBody(position, velocity, glm::quat(), mass)
	{
		m_shapeID = Sphere;
		m_radius = radius;
		
	}
	virtual void MakeGizmo();
};