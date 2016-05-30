#include "DIYPhysicsScene.h"
#include "Gizmos.h"
#include <iostream>
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include <algorithm>
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

void DIYPhysicsScene::Update(GLFWwindow* window, float fDeltaTime)
{
	//Calculate Timestep?
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		gravity.y = -9.8;
		//actors[0]->ApplyForceToActor(actors[1], glm::vec3(1, 1, 1));
		//SphereClass* sphere2 = dynamic_cast<SphereClass*>(actors[0]);
		//sphere2->ApplyForce(glm::vec3(0, 0, 1));
		//std::cout << "Force applied" << std::endl;
		//BoxClass* Box2 = dynamic_cast<BoxClass*>(actors[0]);
		//Box2->m_position.x += 0.1f;
	}
	//CheckForCollisions();

	//Update actors
	for (std::vector<PhysicsObject*>::iterator iter = actors.begin(); iter != actors.end(); ++iter)
	{
		(*iter)->Update(gravity, fDeltaTime);
	}
	CheckForCollisions();
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
	//Check for collisions against all objects, excluding SpringJoints
	for (int outer = 0; outer < actorCount - 1; ++outer)
	{
		if (actors[outer]->m_shapeID != Joint)
		{
			for (int inner = outer + 1; inner < actorCount; ++inner)
			{
				if (actors[inner]->m_shapeID != Joint)
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
	}
}

void DIYPhysicsScene::CollisionSeperate(PhysicsObject* obj1, PhysicsObject* obj2, float overlap, glm::vec3 normal)
{
	float totalMass = obj1->GetMass() + obj2->GetMass();
	float massRatio1 = obj1->GetMass() / totalMass;
	float massRatio2 = obj2->GetMass() / totalMass;
	
	//seperate relative to mass
	glm::vec3 seperationVector = normal * overlap;
	//obj1->Translate(-seperationVector * massRatio2);
	//obj2->Translate(seperationVector * massRatio1);
	if (obj1->m_shapeID != Plane)
	{
		DIYRigidBody* body1 = dynamic_cast<DIYRigidBody*>(obj1);
		body1->m_position += (-seperationVector * massRatio2);
	}
	else
	{
		DIYRigidBody* body2 = dynamic_cast<DIYRigidBody*>(obj2);
		body2->m_position += (seperationVector * massRatio1);
	}

	if (obj2->m_shapeID != Plane)
	{
		DIYRigidBody* body2 = dynamic_cast<DIYRigidBody*>(obj2);
		body2->m_position += (seperationVector * massRatio1);
	}
	else
	{
		DIYRigidBody* body1 = dynamic_cast<DIYRigidBody*>(obj1);
		body1->m_position += (-seperationVector * massRatio2);
	}
	//obj1->m_position += (-seperationVector * massRatio2);
	//obj2->m_position += (seperationVector * massRatio1);
}

void DIYPhysicsScene::CollisionResponse(PhysicsObject* obj1, PhysicsObject* obj2, float overlap, glm::vec3 normal)
{
	//CollisionSeperate(obj1, obj2, overlap, normal);
	//
	//const float coefficientOfRestitution = 0.9f;
	//
	////calculate momentum along collision normal
	//float impulse1, impulse2;
	//if (obj1->m_shapeID == Plane)
	//	impulse1 = 0;
	//	float impulse1 = -(1 + coefficientOfRestitution) * glm::dot(obj1->GetMomentum(), normal);
	//float impulse2 = -(1 + coefficientOfRestitution) * glm::dot(obj2->GetMomentum(), normal);
	//
	////apply change in momentum
	//obj1->AddMomentum(impulse1 * normal);
	//obj2->AddMomentum(impulse2 * normal);
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
			//find the point where collision occured
			//the plane is always static so collision responce only applies to the sphere
			glm::vec3 planeNormal = plane1->m_normal;
			if (sphereToPlane < 0)
			{
				planeNormal *= -1;
			}
			glm::vec3 forceVector = -1 * sphere2->GetMass() * planeNormal * (glm::dot(planeNormal, sphere2->m_velocity));
			sphere2->ApplyForce(2 * forceVector);
			sphere2->m_position += collisionNormal * intersection * 1.0f;

			//CollisionSeperate(obj1, obj2, -intersection, plane1->m_normal);
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
		glm::vec3 boxMin = box2->GetMin();
		glm::vec3 boxMax = box2->GetMax();

		float minPointDistanceAlongPlaneNormal = glm::dot(boxMin, plane1->m_normal);
		float maxPointDistanceAlongPlaneNormal = glm::dot(boxMax, plane1->m_normal);

		float overlap = std::min(minPointDistanceAlongPlaneNormal, maxPointDistanceAlongPlaneNormal);

		if (overlap < 0)
		{
			float boxToPlane = glm::dot(box2->m_position, plane1->m_normal) - plane1->m_originDist;
			glm::vec3 planeNormal = plane1->m_normal;
			if (boxToPlane < 0)
			{
				planeNormal *= -1;
			}
			glm::vec3 forceVector = -1 * box2->GetMass() * planeNormal * (glm::dot(planeNormal, box2->m_velocity));
			box2->ApplyForce(2 * forceVector);
			box2->m_position += planeNormal * std::abs(overlap) * 1.0f;

			std::cout << "box2->Apply force: " << (2 * forceVector.y) << "\tplaneNormal.y: " << planeNormal.y << std::endl;
			
			//CollisionSeperate(obj1, obj2, overlap, plane1->m_normal);
			return true;
		}
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
		glm::vec3 delta = sphere2->m_position - sphere1->m_position;
		float distance = glm::length(delta);
		float intersection = sphere1->m_radius + sphere2->m_radius - distance;
		//if (glm::distance(sphere1->m_position, sphere2->m_position) < (sphere1->m_radius + sphere2->m_radius))
		if (intersection > 0)
		{
			glm::vec3 collisionNormal = glm::normalize(delta);
			glm::vec3 relativeVelocity = sphere1->m_velocity - sphere2->m_velocity;
			glm::vec3 collisionVector = collisionNormal * (glm::dot(relativeVelocity, collisionNormal));
			glm::vec3 forceVector = collisionVector * 1.0f / (1 / sphere1->GetMass() + 1 / sphere2->GetMass());
			//use newtons third law to apply collision forces to colliding bodies
			sphere1->ApplyForceToActor(sphere2, 2 * forceVector);
			//move our sphere out of collisions
			glm::vec3 seperationVector = collisionNormal * intersection * 0.5f;
			sphere1->m_position -= seperationVector;
			sphere2->m_position += seperationVector;
			
			//CollisionSeperate(obj1, obj2, -intersection, collisionNormal);
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
		glm::vec3 box2Min = box2->GetMin();
		glm::vec3 box2Max = box2->GetMax();

		//Clamp sphere and box together
		glm::vec3 distance = sphere1->m_position - box2->m_position;
		glm::vec3 clampPoint = distance;
		
			//clamp x-axis
		if (distance.x < box2Min.x)
			clampPoint.x = box2Min.x;
		else if (distance.x > box2Max.x)
			clampPoint.x = box2Max.x;
			//clamp y-axis
		if (distance.y < box2Min.y)
			clampPoint.y = box2Min.y;
		else if (distance.y > box2Max.y)
			clampPoint.y = box2Max.y;
			//clamp z-axis
		if (distance.z < box2Min.z)
			clampPoint.z = box2Min.z;
		else if (distance.z > box2Max.z)
			clampPoint.z = box2Max.z;

		glm::vec3 clampedDistance = distance - clampPoint;

		//object overlap =  clamped distance - sphere radius
		float overlap = glm::length(clampedDistance) - sphere1->m_radius;

		if (overlap < 0)
		{
			sphere1->ApplyForceToActor(box2, glm::normalize(clampedDistance) * -overlap);
			sphere1->m_position += overlap * glm::normalize(clampedDistance) * 0.5f;
			box2->m_position -= overlap * glm::normalize(clampedDistance) * 0.5f;
			//CollisionSeperate(obj1, obj2, -overlap, glm::normalize(clampedDistance));
			return true;
		}
		return false;
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
		glm::vec3 box1Min = box1->GetMin();
		glm::vec3 box1Max = box1->GetMax();
		glm::vec3 box2Min = box2->GetMin();
		glm::vec3 box2Max = box2->GetMax();

		//glm::vec3 boxDelta = box2->m_position - box1->m_position;
		//std::cout << "BoxDelta: (" << boxDelta.x << ", " << boxDelta.y << ", " << boxDelta.z << ")" << std::endl;

		if (box1Min.x < box2Max.x && box1Max.x > box2Min.x && box1Min.y < box2Max.y && box1Max.y > box2Min.y && box1Min.z < box2Max.z && box1Max.z > box2Min.z)
		{
			glm::vec3 boxDelta = box2->m_position - box1->m_position;
			//boxDelta = box2->m_position - box1->m_position;
			glm::vec3 boxExtents = glm::vec3(box1->m_length/2 + box2->m_length/2, box1->m_height/2 + box2->m_height/2, box1->m_width/2 + box2->m_width/2);
				
			float xOverlap = std::abs(boxDelta.x) - boxExtents.x;
			float yOverlap = std::abs(boxDelta.y) - boxExtents.y;
			float zOverlap = std::abs(boxDelta.z) - boxExtents.z;
			
			//calculate which axis has least overlap
			float minOverlap = xOverlap;
			minOverlap = yOverlap < 0 ? std::max(minOverlap, yOverlap) : minOverlap;
			minOverlap = zOverlap < 0 ? std::max(minOverlap, zOverlap) : minOverlap;
					
			glm::vec3 seperationNormal(0);

			if (xOverlap == minOverlap) seperationNormal.x = std::signbit(boxDelta.x) ? -1.0f : 1.0f;
			else if (yOverlap == minOverlap) seperationNormal.y = std::signbit(boxDelta.y) ? -1.0f : 1.0f;
			else if (zOverlap == minOverlap) seperationNormal.z = std::signbit(boxDelta.z) ? -1.0f : 1.0f;

			//separate
			box1->ApplyForceToActor(box2, seperationNormal * -minOverlap);	
			box1->m_position -= minOverlap * seperationNormal * 0.5f;
			box2->m_position += minOverlap * seperationNormal * 0.5f;
			//CollisionSeperate(obj1, obj2, -minOverlap, seperationNormal);
			return true;
		}
	}
	return false;
}




