#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;

class MyControllerHitReport : public PxUserControllerHitReport
{
public:
	//MyControllerHitReport();
	//~MyControllerHitReport();
	
	//overload the onShapeHit function
	virtual void onShapeHit(const PxControllerShapeHit &hit);
	//other collision functions which we must overlaod
	//these handle collision with other controller and hitting obstacles
	virtual void onControllerHit(const PxControllersHit &hit) {};
	//called when current controller hits another controller. More..
	virtual void onObstacleHit(const PxControllerObstacleHit &hit) {};
	//called when current controller hits a user-defined obstacle
	MyControllerHitReport() : PxUserControllerHitReport() {};
	PxVec3 getPlayerContactNormal() { return _playerContactNormal; };
	void ClearPlayerContactNormal() { _playerContactNormal = PxVec3(0, 0, 0); };
	PxVec3 _playerContactNormal;
};

