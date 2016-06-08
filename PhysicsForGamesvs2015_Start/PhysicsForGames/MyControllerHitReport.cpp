#include "MyControllerHitReport.h"



//MyControllerHitReport::MyControllerHitReport()
//{
//}
//
//
//MyControllerHitReport::~MyControllerHitReport()
//{
//}

void MyControllerHitReport::onShapeHit(const PxControllerShapeHit & hit)
{
	//gets a reference to a structure which tells us what has been hit and where
	//get the actor from the shape we hit
	PxRigidActor* actor = hit.shape->getActor();
	//get the normal of the thing we hit and store it so the player controller can respond correctly
	_playerContactNormal = hit.worldNormal;
	//try to cast to a dynamic actor
	PxRigidDynamic* myActor = actor->is<PxRigidDynamic>();
	if (myActor)
	{
		//this is wehre we cna apply forces to things we hit
	}

	//PxRigidStatic* myStaticActor = actor->is<PxRigidStatic>();
}
