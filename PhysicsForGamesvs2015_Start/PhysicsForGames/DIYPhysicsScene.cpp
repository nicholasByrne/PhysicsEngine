#include "DIYPhysicsScene.h"
#include "Gizmos.h"
#include <iostream>
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"

DIYPhysicsScene::DIYPhysicsScene()
{
}


DIYPhysicsScene::~DIYPhysicsScene()
{
}

void DIYPhysicsScene::AddActor(PhysicsObject* actor)
{
	actors.push_back(actor);
}

void DIYPhysicsScene::RemoveActor(PhysicsObject* actor)
{
}

void DIYPhysicsScene::Update(GLFWwindow* window, float fDeltaTime)
{
	//Calculate Timestep?
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		//actors[0]->ApplyForceToActor(actors[1], glm::vec3(1, 1, 1));
		//std::cout << "Force applied" << std::endl;
	}
	

	//Update actors
	for (std::vector<PhysicsObject*>::iterator iter = actors.begin(); iter != actors.end(); ++iter)
	{
		(*iter)->Update(gravity, fDeltaTime);
	}
	DebugScene();
}

void DIYPhysicsScene::DebugScene()
{
	//std::cout << "Actors: " << actors.size() << std::endl;
}

void DIYPhysicsScene::AddGizmos()
{
	for (std::vector<PhysicsObject*>::iterator iter = actors.begin(); iter != actors.end(); ++iter)
	{
		//if ((*iter)->m_shapeID == Sphere)
		//{
			((*iter)->MakeGizmo());
		//}
		
	}

	Gizmos::addTransform(glm::mat4());
}

void DIYPhysicsScene::CheckForCollisions()
{
	int actorCount = actors.size();
	//Check for collisions against all objects
	for (int outer = 0; outer < actorCount - 1; ++outer)
	{
		for (int inner = outer + 1; inner < actorCount; ++inner)
		{
			PhysicsObject* object1 = actors[outer];
			PhysicsObject* object2 = actors[inner];
			int _shapeID1 = object1->m_shapeID;
			int _shapeID2 = object2->m_shapeID;
			//using function pointers
			int functionIndex = (_shapeID1 * NUMBERSHAPE) + _shapeID2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIndex];
			if (collisionFunctionPtr != NULL)
			{
				collisionFunctionPtr(object1, object2);
			}
		}
	}
}



//Function pointer array for doing our collisions
typedef bool(*fn)(PhysicsObject*, PhysicsObject*);

static fn collisionFunctionArray[] =
{
	DIYPhysicsScene::Plane2Plane,	DIYPhysicsScene::Plane2Sphere,	DIYPhysicsScene::Plane2Box,
	DIYPhysicsScene::Sphere2Plane,	DIYPhysicsScene::Sphere2Sphere,	DIYPhysicsScene::Sphere2Box,
	DIYPhysicsScene::Box2Plane,		DIYPhysicsScene::Box2Sphere,	DIYPhysicsScene::Box2Box,
}
