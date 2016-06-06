#include "Ragdoll.h"



//Ragdoll::Ragdoll()
//{
//}
//
//
//Ragdoll::~Ragdoll()
//{
//}

PxArticulation* RagdollNode::MakeRagdoll(PxPhysics* g_physics, RagdollNode ** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial * ragdollMaterial)
{
	//create the articulation for our ragdoll
	PxArticulation *articulation = g_physics->createArticulation();
	RagdollNode** currentNode = nodeArray;
	//while there are more nodes to process...
	while (*currentNode != NULL)
	{
		//get a point to the current node
		RagdollNode* currentNodePtr = *currentNode;
		//create a pointer ready to hold the parent node pointer if there is one
		RagdollNode* parentNode = nullptr;
		//get scaled cvalues for capsule
		float radius = currentNodePtr->radius * scaleFactor;
		float halfLength = currentNodePtr->halfLength * scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength = 0; //will be set later if there is a parent
		//get a pointer to the parent
		PxArticulationLink* parentLinkPtr = NULL;
		currentNodePtr->scaledGlobalPos = worldPos.p;

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//if there is a parent then we need to work out our local position for hte link
			//get a pointer to the parent node
			parentNode = *(nodeArray + currentNodePtr->parentNodeIdx);
			//get a pointer to the link for the parent
			parentLinkPtr = parentNode->linkPtr;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) * scaleFactor;
			//work out the local position of the node
			PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr->globalRotation.rotate(PxVec3(childHalfLength, 0, 0));
			PxVec3 parentRelative = -currentNodePtr->parentLinkPos * parentNode->globalRotation.rotate(PxVec3(parentHalfLength, 0, 0));
			currentNodePtr->scaledGlobalPos = parentNode->scaledGlobalPos - (parentRelative + currentRelative);
		}

		//Build the transform for the link
		PxTransform linkTransform = PxTransform(currentNodePtr->scaledGlobalPos, currentNodePtr->globalRotation);
		//create the link in the articulation
		PxArticulationLink* link = articulation->createLink(parentLinkPtr, linkTransform);
		//add the pointer to this link into the ragdoll data so we have it for later twhen we want to link to it
		currentNodePtr->linkPtr = link;
		float jointSpace = .01f;
		float capsuleHalfLength = (halfLength, jointSpace ? halfLength - jointSpace : 0) + .01f;
		PxCapsuleGeometry capsule(radius, capsuleHalfLength);
		//add a capsule collider to the link
		link->createShape(capsule, *ragdollMaterial);
		//adds some mass, mass should really be part of the data
		PxRigidBodyExt::updateMassAndInertia(*link, 50.0f);

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//get the pointer to the joint from the link
			PxArticulationJoint *joint = link->getInboundJoint();
			//get the relative rotation of this link
			PxQuat frameRotation = parentNode->globalRotation.getConjugate() * currentNodePtr->globalRotation;
			//set the parent constraint frame
			PxTransform parentConstraintFrame = PxTransform(PxVec3(currentNodePtr->parentLinkPos * parentHalfLength, 0, 0), frameRotation);
			//set the child constraint frame(this is the constraint frame of the newly added link)
			PxTransform thisConstraintFrame = PxTransform(currentNodePtr->childLinkPos * childHalfLength, 0, 0);
			//set up the poses for the joint so it is in the correct place
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);
			//set up some constraints to stop it flopping around
			joint->setStiffness(20);
			joint->setDamping(20);
			joint->setSwingLimit(0.4f, 0.4f);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(0.1f, 0.1f);
			joint->setTwistLimitEnabled(true);
		}

		currentNode++;
	}
	return articulation;
}
