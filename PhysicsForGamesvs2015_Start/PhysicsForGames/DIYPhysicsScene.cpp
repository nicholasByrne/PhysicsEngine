#include "DIYPhysicsScene.h"
#include "Gizmos.h"
#include <iostream>

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

void DIYPhysicsScene::Update(float fDeltaTime)
{
	//Calculate Timestep?

	//Update actors
	for (std::vector<PhysicsObject*>::iterator iter = actors.begin(); iter != actors.end(); ++iter)
	{
		(*iter)->Update(gravity, fDeltaTime);
	}
	DebugScene();
}

void DIYPhysicsScene::DebugScene()
{
	std::cout << "Actors: " << actors.size() << std::endl;
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
}
