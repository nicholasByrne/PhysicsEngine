#pragma once
#include <PxPhysicsAPI.h>
//#include <PxScene.h>
//#include <PxSimulationEventCallback.h>
//#include <pvd\PxVisualDebugger.h>

using namespace physx;

struct FilterGroup
{
	enum Enum
	{
		ePLAYER = (1 << 0),
		ePLATFORM = (1 << 1),
		eGROUND = (1 << 2)
	};
};

class MyCollisionCallBack : public PxSimulationEventCallback
{
public:
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 nbPairs);
	virtual void onConstraintBreak(PxConstraintInfo*, PxU32) {};
	virtual void onWake(PxActor**, PxU32) {};
	virtual void onSleep(PxActor**, PxU32) {};

	static PxFilterFlags myFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
								PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

	//helper function to set up filtering
	static void SetUpPFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);

	static void SetShapeAsTrigger(PxRigidActor* actorIn);

	//MyCollisionCallBack();
	//~MyCollisionCallBack();
};

