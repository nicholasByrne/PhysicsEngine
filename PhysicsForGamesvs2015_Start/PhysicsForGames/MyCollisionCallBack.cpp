#include "MyCollisionCallBack.h"
#include <iostream>
#include "Physics.h"


//MyCollisionCallBack::MyCollisionCallBack()
//{
//}
//
//
//MyCollisionCallBack::~MyCollisionCallBack()
//{
//}

void MyCollisionCallBack::onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];
		//only interested in touches found and lost
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			std::cout << "Collision Detected between: ";
			std::cout << pairHeader.actors[0]->getName();
			std::cout << pairHeader.actors[1]->getName() << std::endl;
		}
	}
}

void MyCollisionCallBack::onTrigger(PxTriggerPair * pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		PxTriggerPair* pair = pairs + i;
		PxActor* triggerActor = pair->triggerActor;
		

		PxActor* otherActor = pair->otherActor;
		const char* pName = otherActor->getName();
		if( pName!=nullptr )
			std::cout << otherActor->getName();
		std::cout << " Entered Trigger ";
		const char* pName1 = triggerActor->getName();
		if (pName1 != nullptr)
			std::cout << triggerActor->getName() << std::endl;
		
		if (pName == "Box Trigger" || pName1 == "Box Trigger")
		{
			Physics* pPhysics = static_cast<Physics*>(triggerActor->userData);
			if (pPhysics != nullptr)
			{
				//pPhysics->StartRagdoll();
				pPhysics->changeRenderColour = true;
			}
		}

	}
}

PxFilterFlags MyCollisionCallBack::myFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags & pairFlags, const void * constantBlock, PxU32 constantBlockSize)
{
	//let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	//generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	//trigger the contact callback for pairs (A, B) where
	//the filtermask of A contains the ID of B and vice versa
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;

	return PxFilterFlag::eDEFAULT;
}

void MyCollisionCallBack::SetUpPFiltering(PxRigidActor * actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; //word0 = own ID
	filterData.word1 = filterMask; //word1 = ID mask to filter pairs that trigger a contact callback
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*) * numShapes, 16);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	_aligned_free(shapes);
}

void MyCollisionCallBack::SetShapeAsTrigger(PxRigidActor * actorIn)
{
	PxRigidStatic* staticActor = actorIn->is<PxRigidStatic>();
	//assert(staticActor);

	const PxU32 numShapes = staticActor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*) * numShapes, 16);
	staticActor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shapes[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
}
