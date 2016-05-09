#pragma once
#include "PhysicsObject.h"
#include "glm\glm.hpp"
#include <vector>

class DIYPhysicsScene
{
public:
	DIYPhysicsScene();
	~DIYPhysicsScene();

	glm::vec3 gravity;
	float timeStep;
	std::vector<PhysicsObject*> actors;
	void AddActor(PhysicsObject* actor);
	void RemoveActor(PhysicsObject* actor);
	void Update(float fDeltaTime);
	void DebugScene();
	void AddGizmos();
};



