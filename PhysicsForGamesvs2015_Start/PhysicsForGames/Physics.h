#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"
#include "Render.h"
#include "PhysxScene.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>
#include "Ragdoll.h"
#include "ParticleEmitter.h"
#include "ParticleFluidEmitter.h"

using namespace physx;
//class PxFoundation;
//class PxPhysics;
//class PhysicsScene;
//class PxDefaultErrorCallback;
//class PxDefaultAllocator;
//class PxSimulationFilterShader;
//class PxMaterial;
//class PxCooking;
class MyControllerHitReport;

class myAllocator : public PxAllocatorCallback
{
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		void* pointer = _aligned_malloc(size, 16);
		return pointer;
	}
	virtual void deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}
};


class Physics : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
    virtual bool update();
    virtual void draw();

	void renderGizmos(PxScene* physics_scene);

    Renderer* m_renderer;
    FlyCamera m_camera;
    float m_delta_time;

	//DIYPhysics
	void DIYPhysicsSetup1();

	DIYPhysicsScene* physicsScene;
	
	//PhysX
	void PhysXSetup();
	void UpdatePhysx(float a_deltaTime);
	void SetUpVisualDebugger();
	void setupTuorial1();

	bool canShoot = true;

	PxFoundation* g_PhysicsFoundation;
	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;
	PxDefaultErrorCallback gDefaultErrorCallBack;
	PxDefaultAllocator gDefaultAllocatorCallBack;
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	PxMaterial* g_PhysicsMaterial;
	PxMaterial* g_boxMaterial;
	PxCooking* g_PhysicsCooker;
	//ParticleEmitter* m_particleEmitter;
	ParticleFluidEmitter* m_particleEmitter;

	MyControllerHitReport* myHitReport;
	PxControllerManager* gCharacterManager;
	PxController* gPlayerController;
	float _characterYVelocity;
	float _characterRotation;
	float _playerGravity;

};



#endif //CAM_PROJ_H_
