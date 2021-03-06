#pragma once
#include "PhysicsObject.h"
#include "glm\glm.hpp"
#include <vector>

struct GLFWwindow;
class FlyCamera;

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
	void Update(GLFWwindow* window, float fDeltaTime, FlyCamera* camera);
	void DebugScene();
	void AddGizmos();

	//Collisions
	void CheckForCollisions();
	static void CollisionSeperate(PhysicsObject*, PhysicsObject*, float overlap, glm::vec3 normal);
	static void CollisionResponse(PhysicsObject*, PhysicsObject*, float overlap, glm::vec3 normal);
	static void DyanmicDynamicCollision(PhysicsObject*, PhysicsObject*, float overlap, glm::vec3 normal);
	static void DyanmicStaticCollision(PhysicsObject*, PhysicsObject*, float overlap, glm::vec3 normal);

	static bool Plane2Plane(PhysicsObject*, PhysicsObject*);
	static bool Plane2Sphere(PhysicsObject*, PhysicsObject*);
	static bool Plane2Box(PhysicsObject*, PhysicsObject*);
	static bool Sphere2Plane(PhysicsObject*, PhysicsObject*);
	static bool Sphere2Sphere(PhysicsObject*, PhysicsObject*);
	static bool Sphere2Box(PhysicsObject*, PhysicsObject*);
	static bool Box2Plane(PhysicsObject*, PhysicsObject*);
	static bool Box2Sphere(PhysicsObject*, PhysicsObject*);
	static bool Box2Box(PhysicsObject*, PhysicsObject*);


	bool canShoot;


};

