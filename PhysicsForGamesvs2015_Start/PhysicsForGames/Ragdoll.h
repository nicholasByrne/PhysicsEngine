#pragma once
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>

using namespace physx;
enum RagDollParts
{
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ART,
};

//Constants for axis of rotation to make definition of quaternions neater
const PxVec3 X_AXIS = PxVec3(1, 0, 0);
const PxVec3 Y_AXIS = PxVec3(0, 1, 0);
const PxVec3 Z_AXIS = PxVec3(0, 0, 1);

struct RagdollNode
{
public:
	//rotation of this link in model space
	PxQuat globalRotation;

	//Position of the link centre in world space which is calculated when the node is precessed
	PxVec3 scaledGlobalPos;

	//Index of the parent node
	int parentNodeIdx;
	//half length of the capsule for this node
	float halfLength;
	//radius of capsule for this node
	float radius;
	//relative pos of link centre in parent to this node. 0 is the centre of the node, -1 is left end of capsule and 1 is right end of capsulse relative to x
	float parentLinkPos;
	//relative position of link centre in child
	float childLinkPos;
	//name of link
	char* name;
	PxArticulationLink* linkPtr;
	//constructor
	RagdollNode(PxQuat _globalRotation, int _parentNodeIdk, float _halfLength, float _radius, float _parentLinkPos, float _childLinkPos, char* _name)
	{
		globalRotation = _globalRotation; parentNodeIdx = _parentNodeIdk; halfLength = _halfLength; radius = _radius; parentLinkPos = _parentLinkPos;
		childLinkPos = _childLinkPos; name = _name;
	};

	static PxArticulation* MakeRagdoll(PxPhysics* g_physics, RagdollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial);
};

//class Ragdoll
//{
//public:
//	Ragdoll();
//	~Ragdoll();
//};

