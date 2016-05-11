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

//Function pointer array for doing our collisions
typedef bool(*fn)(PhysicsObject*, PhysicsObject*);

static fn collisionFunctionArray[] =
{
	DIYPhysicsScene::Plane2Plane,	DIYPhysicsScene::Plane2Sphere,	DIYPhysicsScene::Plane2Box,
	DIYPhysicsScene::Sphere2Plane,	DIYPhysicsScene::Sphere2Sphere,	DIYPhysicsScene::Sphere2Box,
	DIYPhysicsScene::Box2Plane,		DIYPhysicsScene::Box2Sphere,	DIYPhysicsScene::Box2Box,
};

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

bool DIYPhysicsScene::Plane2Plane(PhysicsObject* obj1, PhysicsObject* obj2)
{
	//try to cast objeects to sphere and sphere
	PlaneClass* plane1 = dynamic_cast<PlaneClass*>(obj1);
	PlaneClass* plane2 = dynamic_cast<PlaneClass*>(obj2);
	//if we are successful then test for collision
	if (plane1 != NULL && plane2 != NULL)
	{
		//
	}
	return false;
}

bool DIYPhysicsScene::Plane2Sphere(PhysicsObject* obj1, PhysicsObject* obj2)
{
	//try to cast objeects to sphere and sphere
	PlaneClass* plane1 = dynamic_cast<PlaneClass*>(obj1);
	SphereClass* sphere2 = dynamic_cast<SphereClass*>(obj2);
	//if we are successful then test for collision
	if (plane1 != NULL && sphere2 != NULL)
	{
		glm::vec3 collisionNormal = plane1->m_normal;
		float sphereToPlane = glm::dot(sphere2->m_position, plane1->m_normal) - plane1->m_originDist;
		//If we are behind plane then we flip the normal
		if (sphereToPlane < 0)
		{
			collisionNormal *= -1;
			sphereToPlane *= -1;
		}
		float intersection = sphere2->m_radius - sphereToPlane;
		if (intersection > 0)
		{
			return true;
		}
	}
	return false;
}

bool DIYPhysicsScene::Plane2Box(PhysicsObject* obj1, PhysicsObject* obj2)
{
	//try to cast objeects to sphere and sphere
	PlaneClass* plane1 = dynamic_cast<PlaneClass*>(obj1);
	BoxClass* box2 = dynamic_cast<BoxClass*>(obj2);
	//if we are successful then test for collision
	if (plane1 != NULL && box2 != NULL)
	{

	}
	return false;
}

bool DIYPhysicsScene::Sphere2Plane(PhysicsObject* obj1, PhysicsObject* obj2)
{
	return DIYPhysicsScene::Plane2Sphere(obj2, obj1);
}

bool DIYPhysicsScene::Sphere2Sphere(PhysicsObject* obj1, PhysicsObject* obj2)
{
	//try to cast objeects to sphere and sphere
	SphereClass* sphere1 = dynamic_cast<SphereClass*>(obj1);
	SphereClass* sphere2 = dynamic_cast<SphereClass*>(obj2);
	//if we are successful then test for collision
	if (sphere1 != NULL && sphere2 != NULL)
	{
		if (glm::distance(sphere1->m_position, sphere2->m_position) < (sphere1->m_radius + sphere2->m_radius))
		{
			return true;
		}
	}
	return false;
}

bool DIYPhysicsScene::Sphere2Box(PhysicsObject* obj1, PhysicsObject* obj2)
{
	//try to cast objeects to sphere and sphere
	SphereClass* sphere1 = dynamic_cast<SphereClass*>(obj1);
	BoxClass* box2 = dynamic_cast<BoxClass*>(obj2);
	//if we are successful then test for collision
	if (sphere1 != NULL && box2 != NULL)
	{

	}
	return false;
}

bool DIYPhysicsScene::Box2Plane(PhysicsObject* obj1, PhysicsObject* obj2)
{
	return DIYPhysicsScene::Plane2Box(obj2, obj1);
}

bool DIYPhysicsScene::Box2Sphere(PhysicsObject* obj1, PhysicsObject* obj2)
{
	return DIYPhysicsScene::Sphere2Box(obj2, obj1);
}

bool DIYPhysicsScene::Box2Box(PhysicsObject* obj1, PhysicsObject* obj2)
{
	//try to cast objeects to sphere and sphere
	BoxClass* box1 = dynamic_cast<BoxClass*>(obj1);
	BoxClass* box2 = dynamic_cast<BoxClass*>(obj2);
	//if we are successful then test for collision
	if (box1 != NULL && box2 != NULL)
	{
		glm::vec3 box1Min = glm::vec3(box1->m_position.x - (box1->m_length / 2), box1->m_position.y - (box1->m_height / 2), box1->m_position.z - (box1->m_width / 2));
		glm::vec3 box1Max = glm::vec3(box1->m_position.x + (box1->m_length / 2), box1->m_position.y + (box1->m_height / 2), box1->m_position.z + (box1->m_width / 2));
		glm::vec3 box2Min = glm::vec3(box2->m_position.x - (box2->m_length / 2), box2->m_position.y - (box2->m_height / 2), box2->m_position.z - (box2->m_width / 2));
		glm::vec3 box2Max = glm::vec3(box2->m_position.x + (box2->m_length / 2), box2->m_position.y + (box2->m_height / 2), box2->m_position.z + (box2->m_width / 2));
		if (box1Min.x < box2Max.x && box1Max.x > box2Min.x && box1Min.y < box2Max.y && box1Max.y > box2Min.y && box1Min.z < box2Max.z && box1Max.z > box2Min.z)
		{
			return true;
		}
	}
	return false;
}




