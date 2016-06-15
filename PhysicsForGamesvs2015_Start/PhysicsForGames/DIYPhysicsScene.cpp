#include "DIYPhysicsScene.h"
#include "Gizmos.h"
#include <iostream>
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include <algorithm>
#include <iostream>
#include "Camera.h"

DIYPhysicsScene::DIYPhysicsScene()
{
	canShoot = true;
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

void DIYPhysicsScene::Update(GLFWwindow* window, float fDeltaTime, FlyCamera* camera)
{
	if (glfwGetKey(window, GLFW_KEY_Z))
	{
		if (canShoot)
		{
			canShoot = false;
			SphereClass* newBox0;
			newBox0 = new SphereClass(glm::vec3(camera->world[3]), glm::vec3(-camera->world[2] * 40), 1, 1, glm::vec4(0), false);
			AddActor(newBox0);
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_X))
	{
		if (canShoot)
		{
			canShoot = false;
			BoxClass* newBox0;
			newBox0 = new BoxClass(glm::vec3(camera->world[3]), glm::vec3(-camera->world[2] *  40), 1, 1, 1, 1, glm::vec4(0), false);
			AddActor(newBox0);
		}
	}
	else
		canShoot = true;


	//Update actors
	int i = 0;
	for (std::vector<PhysicsObject*>::iterator iter = actors.begin(); iter != actors.end(); ++iter)
	{
		(*iter)->Update(gravity, fDeltaTime);
		i++;
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
	
	if (obj1->GetStaticValue())
	{
		massRatio1 = 0;
		massRatio2 = 1;
	}
	else if (obj2->GetStaticValue())
	{
		massRatio1 = 1;
		massRatio2 = 0;
	}

	//seperate relative to mass
	glm::vec3 seperationVector = normal * overlap;

	DIYRigidBody* body1 = dynamic_cast<DIYRigidBody*>(obj1);
	if (body1 != nullptr)
		body1->m_position += (-seperationVector * massRatio1);

	DIYRigidBody* body2 = dynamic_cast<DIYRigidBody*>(obj2);
	if (body2 != nullptr)
		body2->m_position += (seperationVector * massRatio2);
}

void DIYPhysicsScene::CollisionResponse(PhysicsObject* obj1, PhysicsObject* obj2, float overlap, glm::vec3 normal)
{
	//Static && Static
	if ((obj1->GetStaticValue() == true) && (obj2->GetStaticValue() == true))
		return;

	//Seperate
	CollisionSeperate(obj1, obj2, overlap, normal);
	
	
	//Dynamic && Static
	if (obj2->GetStaticValue())
		DyanmicStaticCollision(obj1, obj2, overlap, normal);
	else if (obj1->GetStaticValue())
		DyanmicStaticCollision(obj2, obj1, overlap, -normal);
	//Dynamic && Dynamic
	else 
		DyanmicDynamicCollision(obj1, obj2, overlap, normal);
}

void DIYPhysicsScene::DyanmicDynamicCollision(PhysicsObject* obj1, PhysicsObject* obj2, float overlap, glm::vec3 normal)
{
	DIYRigidBody* object1 = dynamic_cast<DIYRigidBody*>(obj1);
	DIYRigidBody* object2 = dynamic_cast<DIYRigidBody*>(obj2);

	glm::vec3 relativeVelocity = object2->m_velocity - object1->m_velocity;

	float velocityAlongNormal = glm::dot(relativeVelocity, normal);
	float impuseAmount = -(2) * velocityAlongNormal;
	//					 -(1 + CoefficientOfRestitution) * velocityAlongNormal
	impuseAmount /= 1 / object1->GetMass() + 1 / object2->GetMass();

	glm::vec3 impulse = impuseAmount * normal * 0.9f;
	object1->m_velocity += (1 / object1->GetMass() * -impulse);
	object2->m_velocity += (1 / object2->GetMass() * +impulse);
}

void DIYPhysicsScene::DyanmicStaticCollision(PhysicsObject* obj1, PhysicsObject* obj2, float overlap, glm::vec3 normal)
{
	//obj 1 = dynamic, obj2 = static
	DIYRigidBody* object1 = dynamic_cast<DIYRigidBody*>(obj1);
	glm::vec3 relativeVelocity = object1->m_velocity;

	float velocityAlongNormal = glm::dot(relativeVelocity, normal);
	float impulseAmount = -(-2) * velocityAlongNormal;
	//					 -(1 + CoefficientOfRestitution) * velocityAlongNormal
	impulseAmount /= (1 / obj1->GetMass() + 1 / obj2->GetMass());

	glm::vec3 impulse = impulseAmount * normal * 0.9f;
	object1->m_velocity += (1 / object1->GetMass() * -impulse);
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
			//glm::vec3 forceVector = -1 * sphere2->GetMass() * planeNormal * (glm::dot(planeNormal, sphere2->m_velocity));
			//sphere2->ApplyForce(2 * forceVector);
			//sphere2->m_position += collisionNormal * intersection * 1.0f;

			//glm::vec3 forceVector = -1.8 * planeNormal * (glm::dot(planeNormal, sphere2->m_velocity));
			//sphere2->m_velocity += forceVector;
			//sphere2->m_position += planeNormal * std::abs(intersection) * 1.0f;


			CollisionResponse(obj1, obj2, intersection, planeNormal);
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
	if (plane1 != NULL && box2 != NULL && !box2->GetStaticValue())
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
			CollisionResponse(obj1, obj2, -overlap, plane1->m_normal);
			//std::cout << overlap << std::endl;
			//glm::vec3 forceVector = -2 * planeNormal * (glm::dot(planeNormal, box2->m_velocity));
			//box2->m_velocity += forceVector;
			//box2->m_position += planeNormal * std::abs(overlap) * 1.0f;
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
			CollisionResponse(sphere1, sphere2, intersection, collisionNormal);
			//sphere1->ApplyForceToActor(sphere2, 2 * forceVector);
			////move our sphere out of collisions
			//glm::vec3 seperationVector = collisionNormal * intersection * 0.5f;
			//sphere1->m_position -= seperationVector;
			//sphere2->m_position += seperationVector;
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
		glm::vec3 box2MinExtents = -box2->GetExtents();
		glm::vec3 box2MaxExtents = box2->GetExtents();

		//Clamp sphere and box together
		glm::vec3 distance = sphere1->m_position - box2->m_position;
		glm::vec3 clampPoint = distance;
		
			//clamp x-axis
		if (distance.x < box2MinExtents.x)
			clampPoint.x = box2MinExtents.x;
		else if (distance.x > box2MaxExtents.x)
			clampPoint.x = box2MaxExtents.x;
			//clamp y-axis
		if (distance.y < box2MinExtents.y)
			clampPoint.y = box2MinExtents.y;
		else if (distance.y > box2MaxExtents.y)
			clampPoint.y = box2MaxExtents.y;
			//clamp z-axis
		if (distance.z < box2MinExtents.z)
			clampPoint.z = box2MinExtents.z;
		else if (distance.z > box2MaxExtents.z)
			clampPoint.z = box2MaxExtents.z;

		glm::vec3 clampedDistance = distance - clampPoint;

		//object overlap =  clamped distance - sphere radius
		float overlap = glm::length(clampedDistance) -sphere1->m_radius;

		if (overlap < 0)
		{
			glm::vec3 collisionNormal = glm::normalize(distance);
			glm::vec3 relativeVelocity = sphere1->m_velocity - box2->m_velocity;
			glm::vec3 collisionVector = collisionNormal * (glm::dot(relativeVelocity, collisionNormal));
			glm::vec3 forceVector = collisionVector * 1.0f / (1 / sphere1->GetMass() + 1 / box2->GetMass());
			//Newtons Third Law
			CollisionResponse(box2, sphere1, -overlap, glm::normalize(clampedDistance));

			//sphere1->ApplyForceToActor(box2, forceVector * 2);
			////Seperate Objects
			//if (sphere1->GetStaticValue())
			//	box2->m_position += overlap * glm::normalize(clampedDistance) * 0.5f;
			//else 
			//	sphere1->m_position -= overlap * glm::normalize(clampedDistance) * 0.5f;
			//
			//if (box2->GetStaticValue())
			//	sphere1->m_position -= overlap * glm::normalize(clampedDistance) * 0.5f;
			//else
			//	box2->m_position += overlap * glm::normalize(clampedDistance) * 0.5f;




			//sphere1->m_position -= overlap * glm::normalize(clampedDistance) * 0.5f;
			//box2->m_position += overlap * glm::normalize(clampedDistance) * 0.5f;
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
			//boxDelta = box2->m_position - box1->m_position;
			glm::vec3 boxDelta = box2->m_position - box1->m_position;
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

			//PushAway
			CollisionResponse(obj1, obj2, -minOverlap, seperationNormal);

			//glm::vec3 collisionNormal = seperationNormal;// glm::normalize(boxDelta * seperationNormal);
			//glm::vec3 relativeVelocity = box2->m_velocity - box1->m_velocity;
			//glm::vec3 collisionVector = collisionNormal * (glm::dot(relativeVelocity, collisionNormal));
			//glm::vec3 forceVector = collisionVector * 1.0f / (1 / box1->GetMass() + 1 / box2->GetMass());
			//box1->m_velocity -= (1/box1->GetMass()) * forceVector;
			//box2->m_velocity += (1 / box2->GetMass()) * forceVector;

			//glm::vec3 forceVector = -1 * sphere2->GetMass() * planeNormal * (glm::dot(planeNormal, sphere2->m_velocity));

			//std::cout << "AABB-AABB Collision!" << std::endl;
			//std::cout << "OLD Box1Pos = (" << box1->m_position.x << ",\t " << box1->m_position.y << ",\t " << box1->m_position.z << ") - " << std::endl;
			//std::cout << "OLD Box2Pos = (" << box2->m_position.x << ",\t " << box2->m_position.y << ",\t " << box2->m_position.z << ") - " << std::endl;
			//box1->ApplyForceToActor(box2, seperationNormal * -minOverlap);	
			////Seperate
			//box1->m_position += minOverlap * collisionNormal * 0.5f;
			//box2->m_position -= minOverlap * collisionNormal * 0.5f;

			//CollisionSeperate(obj1, obj2, -minOverlap, seperationNormal);
			//std::cout << "NEW Box1Pos = (" << box1->m_position.x << ",\t " << box1->m_position.y << ",\t " << box1->m_position.z << ") - " << std::endl;
			//std::cout << "NEW Box2Pos = (" << box2->m_position.x << ",\t " << box2->m_position.y << ",\t " << box2->m_position.z << ") - " << std::endl;
			return true;
		}
	}
	return false;
}




// P2S P2B
// S2S S2B
// 