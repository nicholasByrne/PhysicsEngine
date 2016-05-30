#pragma once
#include "glm\glm.hpp"
#include "glm\ext.hpp"

enum ShapeType
{
	Plane = 0,
	Sphere = 1,
	Box = 2,
	NUMBERSHAPE = 3,
	Joint = 4,
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
	void SetStaticValue(bool value);
	bool GetStaticValue() { return m_static; }
	float GetMass();
	void SetMass(float _mass) { m_mass = _mass; }

private:
	bool m_static = false;
	float m_mass;
};


class DIYRigidBody : public PhysicsObject
{
public:
	DIYRigidBody(glm::vec3 position, glm::vec3 velocity, glm::quat rotation, float mass, bool isStatic);

	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;

	//float m_mass;
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
	SphereClass(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour, bool isStatic = false)
		: DIYRigidBody(position, velocity, glm::quat(), mass, isStatic)
	{
		m_shapeID = Sphere;
		m_radius = radius;
		
	}
	virtual void Update(glm::vec3 gravity, float timeStep);
	virtual void Debug();
	virtual void MakeGizmo();
};


class PlaneClass : public PhysicsObject
{
public:
	PlaneClass(glm::vec3 normal, float DistanceFromOrigin);
	
	void virtual Update(glm::vec3 gravity, float timeStep);
	void virtual Debug();
	void virtual MakeGizmo();

	glm::vec3 m_normal;
	float m_originDist;
};


class BoxClass : public DIYRigidBody
{
public:
	//BoxClass();
	BoxClass(glm::vec3 position, glm::vec3 velocity, float mass, float length, float height, float width, glm::vec4 colour, bool isStatic = false) 
		: DIYRigidBody(position, velocity, glm::quat(), mass, isStatic)
	{
		m_shapeID = Box;
		m_length = length;
		m_height = height;
		m_width = width;
	}

	void virtual Update(glm::vec3 gravtiy, float timeStep);
	void virtual Debug();
	void virtual MakeGizmo();

	glm::vec3 GetMin() { return glm::vec3(m_position.x - (m_length / 2), m_position.y - (m_height / 2), m_position.z - (m_width / 2)); }
	glm::vec3 GetMax() { return glm::vec3(m_position.x + (m_length / 2), m_position.y + (m_height / 2), m_position.z + (m_width / 2)); }

	float m_length;
	float m_height;
	float m_width;
};


class SpringJoint : public PhysicsObject
{
public:
	SpringJoint(DIYRigidBody* connection1, DIYRigidBody* connection2, float springCoefficient, float damping);

	void virtual Update(glm::vec3 gravity, float timeStep);
	void virtual Debug();
	void virtual MakeGizmo();

private:
	DIYRigidBody* m_connections[2];
	float m_damping;
	float m_restLength;
	float m_springCoefficient;

};

//DIYPhysicsScene Collision Seperation and Response