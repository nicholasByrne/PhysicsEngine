#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/ext.hpp"
#include "glm/gtc/quaternion.hpp"

#include "PhysicsObject.h"
#include "MyCollisionCallBack.h"

#include "MyControllerHitReport.h"
#include "physx\characterkinematic\PxCapsuleController.h"
#include "physx\characterkinematic\PxController.h"
#include "physx\characterkinematic\PxControllerManager.h"

//#include <cstdlib> //rand


#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))

bool Physics::startup()
{
    if (Application::startup() == false)
    {
        return false;
    }
	
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    Gizmos::create();

    m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
    m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	m_renderer = new Renderer();
	geo_color = vec4(1, 0, 0, 1);
	changeRenderColour = false;
	//MyControllerHitReport::RenderColour = geo_color;

	DIYPhysicsSetup1();
	PhysXSetup();
	SetUpVisualDebugger();
	setupTuorial1();

    return true;
}

void Physics::shutdown()
{

	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();

	delete m_renderer;
    Gizmos::destroy();
    Application::shutdown();
}

bool Physics::update()
{
    if (Application::update() == false)
    {
        return false;
    }

    Gizmos::clear();

    float dt = (float)glfwGetTime();
    m_delta_time = dt;
    glfwSetTime(0.0);

    vec4 white(1);
    vec4 black(0, 0, 0, 1);

    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
            i == 10 ? white : black);
    }

    m_camera.update(1.0f / 60.0f);

	//Update Custom Physics
	//physicsScene->Update(m_window, m_delta_time);

	//Update Physx
	UpdatePhysx(m_delta_time);

    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
    //Gizmos::draw(m_camera.proj, m_camera.view);

    m_renderer->RenderAndClear(m_camera.view_proj);

	//Draw Custom Physics
	//physicsScene->AddGizmos();


	//Gizmos::addSphere(glm::vec3(0, 0, 0), 1, 8, 8, glm::vec4(1, 0, 1, 1));
	//Gizmos::addSphere(glm::vec3(0, 0, 0), 1, 8, 8, glm::vec4(1, 1, 0, 1));
	//Gizmos::addSphereFilled(glm::vec3(0, 0, 0), 1, 8, 8, glm::vec4(1, 1, 0, 1));

	Gizmos::draw(m_camera.proj, m_camera.view);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void AddWidget(PxShape* shape, PxRigidActor* actor, vec4 geo_color)
{
    PxTransform full_transform = PxShapeExt::getGlobalPose(*shape, *actor);
    vec3 actor_position(full_transform.p.x, full_transform.p.y, full_transform.p.z);
    glm::quat actor_rotation(full_transform.q.w,
        full_transform.q.x,
        full_transform.q.y,
        full_transform.q.z);
    glm::mat4 rot(actor_rotation);

    mat4 rotate_matrix = glm::rotate(10.f, glm::vec3(7, 7, 7));

    PxGeometryType::Enum geo_type = shape->getGeometryType();

    switch (geo_type)
    {
    case (PxGeometryType::eBOX) :
    {
        PxBoxGeometry geo;
        shape->getBoxGeometry(geo);
        vec3 extents(geo.halfExtents.x, geo.halfExtents.y, geo.halfExtents.z);
        Gizmos::addAABBFilled(actor_position, extents, geo_color, &rot);
    } break;
    case (PxGeometryType::eCAPSULE) :
    {
        PxCapsuleGeometry geo;
        shape->getCapsuleGeometry(geo);
        Gizmos::addCapsule(actor_position, geo.halfHeight * 2, geo.radius, 16, 16, geo_color, &rot);
    } break;
    case (PxGeometryType::eSPHERE) :
    {
        PxSphereGeometry geo;
        shape->getSphereGeometry(geo);
        Gizmos::addSphereFilled(actor_position, geo.radius, 16, 16, geo_color, &rot);
    } break;
    case (PxGeometryType::ePLANE) :
    {

    } break;
    }
}

void Physics::renderGizmos(PxScene* physics_scene)
{
    PxActorTypeFlags desiredTypes = PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC;
    PxU32 actor_count = physics_scene->getNbActors(desiredTypes);
    PxActor** actor_list = new PxActor*[actor_count];
	physics_scene->getActors(desiredTypes, actor_list, actor_count);
    
    //vec4 geo_color(1, 0, 0, 1);
	//geo_color = MyControllerHitReport::RenderColour;
	if (changeRenderColour)
	{
		geo_color = glm::vec4((float)(rand() % 100) / 100, (float)(rand() % 100) / 100, (float)(rand() % 100) / 100, 1);
		changeRenderColour = false;
	}

    for (int actor_index = 0;
        actor_index < (int)actor_count;
        ++actor_index)
    {
        PxActor* curr_actor = actor_list[actor_index];
        if (curr_actor->isRigidActor())
        {
            PxRigidActor* rigid_actor = (PxRigidActor*)curr_actor;
            PxU32 shape_count = rigid_actor->getNbShapes();
            PxShape** shapes = new PxShape*[shape_count];
            rigid_actor->getShapes(shapes, shape_count);

            for (int shape_index = 0;
                shape_index < (int)shape_count;
                ++shape_index)
            {
                PxShape* curr_shape = shapes[shape_index];
                AddWidget(curr_shape, rigid_actor, geo_color);
            }

            delete[]shapes;
        }
    }

    delete[] actor_list;

    int articulation_count = physics_scene->getNbArticulations();

    for (int a = 0; a < articulation_count; ++a)
    {
        PxArticulation* articulation;
		physics_scene->getArticulations(&articulation, 1, a);

        int link_count = articulation->getNbLinks();

        PxArticulationLink** links = new PxArticulationLink*[link_count];
        articulation->getLinks(links, link_count);

        for (int l = 0; l < link_count; ++l)
        {
            PxArticulationLink* link = links[l];
            int shape_count = link->getNbShapes();

            for (int s = 0; s < shape_count; ++s)
            {
                PxShape* shape;
                link->getShapes(&shape, 1, s);
                AddWidget(shape, link, geo_color);
            }
        }
        delete[] links;
    }
}

void Physics::DIYPhysicsSetup1()
{
	physicsScene = new DIYPhysicsScene();
	physicsScene->gravity = glm::vec3(0, -9.8f, 0);
	physicsScene->timeStep = .001f;

	//add four balls to the simulation
	SphereClass* newBall0;
	newBall0 = new SphereClass(glm::vec3(0, 4, -8), glm::vec3(0, 0, 3), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall0);
	
	//SphereClass* newBall1;
	//newBall1 = new SphereClass(glm::vec3(0, 4, 10), glm::vec3(0, 0, -3), 3.0f, 1, glm::vec4(1, 0, 0, 1), false);
	//physicsScene->AddActor(newBall1);
	//
	//BoxClass* newBox00;
	//newBox00 = new BoxClass(glm::vec3(8, 2.f, 1), glm::vec3(-4.f, 0, 0), 3.0f, 1.f, 1.f, 1.f, glm::vec4(1, 0, 0, 1), false);
	//physicsScene->AddActor(newBox00);
	////
	//BoxClass* newBox11;
	//newBox11 = new BoxClass(glm::vec3(-8.f, 2.f, 1), glm::vec3(4, 0, 0), 3.0f, 1.f, 1.f, 2.f, glm::vec4(1, 0, 0, 1), false);
	//physicsScene->AddActor(newBox11);
	
	//BoxClass* newBox12;
	//newBox12 = new BoxClass(glm::vec3(-3.0f, 2.2f, 0), glm::vec3(0.8f, 0, 0), 3.0f, 1.f, 1.f, 2.f, glm::vec4(1, 0, 0, 1), false);
	//physicsScene->AddActor(newBox12);

	
	

	//Floor
	PlaneClass* newPlane0;
	newPlane0 = new PlaneClass(glm::normalize(glm::vec3(0, 1, 0)), 0);
	physicsScene->AddActor(newPlane0);

	//Boundaries
	BoxClass* newBox0;
	newBox0 = new BoxClass(glm::vec3(20, 0, 0), glm::vec3(0, 0, 0), 10000, 2, 8, 40, glm::vec4(0), true);
	physicsScene->AddActor(newBox0);
	BoxClass* newBox1;
	newBox1 = new BoxClass(glm::vec3(-20, 0, 0), glm::vec3(0, 0, 0), 10000, 2, 8, 40, glm::vec4(0), true);
	physicsScene->AddActor(newBox1);
	BoxClass* newBox2;
	newBox2 = new BoxClass(glm::vec3(0, 0, 20), glm::vec3(0, 0, 0), 10000, 38, 8, 2, glm::vec4(0), true);
	physicsScene->AddActor(newBox2);
	BoxClass* newBox3;
	newBox3 = new BoxClass(glm::vec3(0, 0, -20), glm::vec3(0, 0, 0), 10000, 38, 8, 2, glm::vec4(0), true);
	physicsScene->AddActor(newBox3);


	//int numberBalls = 10;
	SphereClass* ball1 = new SphereClass(glm::vec3(1, 20, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), true);
	SphereClass* ball2 = new SphereClass(glm::vec3(1, 19, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), false);
	//SphereClass* ball3 = new SphereClass(glm::vec3(1, 3, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), false);
	//SphereClass* ball4 = new SphereClass(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), false);
	SpringJoint* joint1 = new SpringJoint(ball1, ball2, 5.f, 0.5f);
	//SpringJoint* joint2 = new SpringJoint(ball2, ball3, 0.5f, 0.2f);
	//SpringJoint* joint3 = new SpringJoint(ball3, ball4, 0.5f, 0.2f);
	//
	physicsScene->AddActor(ball1);
	physicsScene->AddActor(ball2);
	//physicsScene->AddActor(ball3);
	//physicsScene->AddActor(ball4);
	physicsScene->AddActor(joint1);
	//physicsScene->AddActor(joint2);
	//physicsScene->AddActor(joint3);

	//for (int i = 0; i < numberBalls; ++i)
	//{
	//	ball2 = new SphereClass(glm::vec3(1.f, numberBalls * 3 - i * 2, 1), glm::vec3(0, 0, 0), 3.0f, 0.4f, glm::vec4(1, 0, 0, 1));
	//	physicsScene->AddActor(ball2);
	//	SpringJoint* spring = new SpringJoint(ball1, ball2, .5f, 0.9f);
	//	physicsScene->AddActor(spring);
	//	ball1 = ball2;
	//}



	const int width = 4;
	const int height = 4;
	
	SphereClass* ball[width][height];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ball[i][j] = new SphereClass(glm::vec3(2 * i, 2 * j + 2, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), false);
			physicsScene->AddActor(ball[i][j]);
		}
	}
	ball[0][height-1]->SetStaticValue(true);
	ball[width - 1][height - 1]->SetStaticValue(true);
	
	SpringJoint* joint[24];
	int jointCount = 0;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			joint[jointCount] = new SpringJoint(ball[i][j], ball[i][j + 1], 20, 0.9);
			jointCount++;
		}
	}
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			joint[jointCount] = new SpringJoint(ball[i][j], ball[i + 1][j], 20, 0.9); 
			jointCount++;
		}
	}
	//physicsScene->AddActor(joint[jointCount]);
	for (int i = 0; i < jointCount; ++i)
	{
		physicsScene->AddActor(joint[i]);
	}
}

void Physics::PhysXSetup()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallBack);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	//create Physics Material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -10.0f, 0);
	sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);
	//Trigger callback
	PxSimulationEventCallback* myCollisionCallBack = new MyCollisionCallBack();
	g_PhysicsScene->setSimulationEventCallback(myCollisionCallBack);
	sceneDesc.filterShader = MyCollisionCallBack::myFilterShader; //how does this work?

}

void Physics::UpdatePhysx(float a_deltaTime)
{
	if (a_deltaTime <= 0)
		return;
	if (glfwGetKey(m_window, GLFW_KEY_1))
	{
		if (canShoot)
		{
			canShoot = false;
			//Transform
			glm::vec3 cam_pos = glm::vec3(m_camera.world[3]);
			glm::vec3 box_vel = glm::vec3(-m_camera.world[2]);
			PxTransform box_transform(PxVec3(cam_pos.x, cam_pos.y, cam_pos.z));
			//geometry
			PxSphereGeometry sphere(0.5f);
			//Density
			float density = 10;
			//Material 
			PxRigidDynamic* new_actor = PxCreateDynamic(*g_Physics, box_transform, sphere, *g_PhysicsMaterial, density);
			glm::vec3 direction(m_camera.world[2]);
			physx::PxVec3 velocity = physx::PxVec3(direction.x, direction.y, direction.z) * -40;
			new_actor->setLinearVelocity(velocity, true);
			new_actor->setName("Sphere");
			MyCollisionCallBack::SetUpPFiltering(new_actor, FilterGroup::ePLAYER, FilterGroup::eGROUND); //sets up collision filtering for our player
			g_PhysicsScene->addActor(*new_actor);
		}
	}
	else if (glfwGetKey(m_window, GLFW_KEY_2))
	{
		if (canShoot)
		{
			canShoot = false;
			//Transform
			glm::vec3 cam_pos = glm::vec3(m_camera.world[3]);
			glm::vec3 box_vel = glm::vec3(-m_camera.world[2]);
			PxTransform box_transform(PxVec3(cam_pos.x, cam_pos.y, cam_pos.z));
			//geometry
			PxBoxGeometry box(0.5f, 0.5f, 0.5f);
			//Density
			float density = 10;
			//Material 
			PxRigidDynamic* new_actor = PxCreateDynamic(*g_Physics, box_transform, box, *g_PhysicsMaterial, density);
			glm::vec3 direction(m_camera.world[2]);
			physx::PxVec3 velocity = physx::PxVec3(direction.x, direction.y, direction.z) * -40;
			new_actor->setLinearVelocity(velocity, true);
			new_actor->setName("Box");
			MyCollisionCallBack::SetUpPFiltering(new_actor, FilterGroup::eGROUND, FilterGroup::ePLAYER); //set up collision filtering for ground
			g_PhysicsScene->addActor(*new_actor);
		}
	}
	else if (glfwGetKey(m_window, GLFW_KEY_3))
	{
		if (canShoot)
		{
			canShoot = false;
			//Transform
			glm::vec3 cam_pos = glm::vec3(m_camera.world[3]);
			glm::vec3 box_vel = glm::vec3(-m_camera.world[2]);
			PxTransform box_transform(PxVec3(cam_pos.x, cam_pos.y, cam_pos.z));
			//geometry
			PxCapsuleGeometry capsule(0.5f, 2.f);
			//Density
			float density = 10;
			//Material 
			PxRigidDynamic* new_actor = PxCreateDynamic(*g_Physics, box_transform, capsule, *g_PhysicsMaterial, density);
			glm::vec3 direction(m_camera.world[2]);
			physx::PxVec3 velocity = physx::PxVec3(direction.x, direction.y, direction.z) * -40;
			new_actor->setLinearVelocity(velocity, true);
			new_actor->setName("Capsule");
			MyCollisionCallBack::SetUpPFiltering(new_actor, FilterGroup::ePLAYER, FilterGroup::eGROUND | FilterGroup::ePLATFORM); //set up collision filtering for our player with ground and platform
			g_PhysicsScene->addActor(*new_actor);
		}
	}
	else
		canShoot = true;

	//Update Player Controller
	bool onGround; //set to true if we are on the ground
	float movementSpeed = 10.0f; //forward and back movement speed
	float rotationSpeed = 1.0f; //turn speed
	//check if we have contact normal. if y is greater than .3 we assume this is solid ground
	if (myHitReport->getPlayerContactNormal().y > 0.3f)
	{
		_characterYVelocity = -0.1f;
		onGround = true;
	}
	else
	{
		_characterYVelocity += _playerGravity * a_deltaTime;
		onGround = false;
	}
	myHitReport->ClearPlayerContactNormal();
	const PxVec3 Up(0, 1, 0);
	//scan the keys and set up our intended velocity based on a global transform
	PxVec3 velocity(0, _characterYVelocity, 0);
	//if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
	if ((glfwGetKey(m_window, GLFW_KEY_UP)))
	{
		velocity.x -= movementSpeed * a_deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN))
	{
		velocity.x += movementSpeed * a_deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT))
	{
		velocity.z += movementSpeed * a_deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT))
	{
		velocity.z -= movementSpeed * a_deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) && onGround)
	{
		velocity.y += 10 * movementSpeed * a_deltaTime;
	}
	//todo add z movement and jumping
	float minDistance = 0.001f;
	PxControllerFilters filter;
	//make controls relative to player facing
	PxQuat rotation(_characterRotation, PxVec3(0, 1, 0));
	//PxVec3 velocity1(0, _characterYVelocity, 0);
	//move the controller
	gPlayerController->move(rotation.rotate(velocity), minDistance, a_deltaTime, filter);


	//Update fluid dynamics
	if (m_particleEmitter)
	{
		m_particleEmitter->update(a_deltaTime);
		//render all our particles
		m_particleEmitter->renderParticles();
	}

	g_PhysicsScene->simulate(a_deltaTime);
	while (g_PhysicsScene->fetchResults() == false)
	{

	}
	renderGizmos(g_PhysicsScene);
}

void Physics::SetUpVisualDebugger()
{
	//check if PvdConnection manager is available on this platform
	if (g_Physics->getPvdConnectionManager() == NULL)
		return;
	//set up connection parameters
	const char* pvd_host_ip = "127.0.0.1";
	//IP of the PC which is running PVD
	int port = 5425;
	//TCP port to connect to, where PVD is listening
	unsigned int timeout = 100;
	//timeout in milliseconds to wait for PVD to respond,
	//consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	// and now try to connectPxVisualDebuggerExt
	auto theConnection = PxVisualDebuggerExt::createConnection(g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
}

void Physics::setupTuorial1()
{
	
	//add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi * 1.0f, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);
	plane->userData = this;
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);

	//add a box
	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(0, 5, 0));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box, *g_PhysicsMaterial, density);
	dynamicActor->userData = this;
	//MyCollisionCallBack::SetShapeAsTrigger(dynamicActor);
	//add it to the PhysX scene
	g_PhysicsScene->addActor(*dynamicActor);

	//add a box
	PxBoxGeometry box1(2, 2, 2);
	PxTransform transform1(PxVec3(20, 5, 0));
	PxRigidStatic* staticActor = PxCreateStatic(*g_Physics, transform1, box1, *g_PhysicsMaterial);
	//PxRigidStatic* dynamicActor = PxCreateStatic(*g_Physics, transform, box, *g_PhysicsMaterial, density);
	staticActor->setName("Box Trigger");
	MyCollisionCallBack::SetShapeAsTrigger(staticActor);
	staticActor->userData = this;
	//add it to the PhysX scene
	g_PhysicsScene->addActor(*staticActor);

	RagdollNode* ragdollData[] =
	{
		new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NO_PARENT,1,3,1,1,"lower spine"),
		new RagdollNode(PxQuat(PxPi, Z_AXIS), LOWER_SPINE, 1,1,-1,1,"left pelvis"),
		new RagdollNode(PxQuat(0, Z_AXIS), LOWER_SPINE, 1,1,-1, 1,"right pelvis"),
		new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS),LEFT_PELVIS,5,2,-1,1,"L upper leg"),
		new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS),RIGHT_PELVIS,5,2,-1,1,"R upper leg"),
		new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS),LEFT_UPPER_LEG,5,1.75,-1,1,"L lower leg"),
		new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS),RIGHT_UPPER_LEG,5,1.75,-1,1,"R lowerleg"),
		new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), LOWER_SPINE, 1, 3, 1, -1, "upper spine"),
		new RagdollNode(PxQuat(PxPi, Z_AXIS), UPPER_SPINE, 1, 1.5, 1, 1, "left clavicle"),
		new RagdollNode(PxQuat(0, Z_AXIS), UPPER_SPINE, 1, 1.5, 1, 1, "right clavicle"),
		new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), UPPER_SPINE, 1, 1, 1, -1, "neck"),
		new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NECK, 1, 3, 1, -1, "HEAD"),
		new RagdollNode(PxQuat(PxPi - .3, Z_AXIS), LEFT_CLAVICLE, 3, 1.5, -1, 1, "left upper arm"),
		new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_CLAVICLE, 3, 1.5, -1, 1, "right upper arm"),
		new RagdollNode(PxQuat(PxPi - .3, Z_AXIS), LEFT_UPPER_ARM, 3, 1, -1, 1, "left lower arm"),
		new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_UPPER_ARM, 3, 1, -1, 1, "right lower arm"),
		NULL
	};

	PxArticulation* ragdollArticulation;
	ragdollArticulation = RagdollNode::MakeRagdoll(g_Physics, ragdollData, PxTransform(PxVec3(5, 2, 10)), .1f, g_PhysicsMaterial);
	ragdollArticulation->userData = this;
	g_PhysicsScene->addArticulation(*ragdollArticulation);

	//Fluid dynamics
	//PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	//PxRigidStatic* plane1 = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);
	//const PxU32 numShapes = plane1->getNbShapes();

	//PxBoxGeometry side1(4.5, 1, 0.5);
	//PxBoxGeometry side2(.5, 1, 4.5);
	//pose = PxTransform(PxVec3(0.0f, 0.5, 4.0f));
	//PxRigidStatic* Box = PxCreateStatic(*g_Physics, pose, side1, *g_PhysicsMaterial);

	//g_PhysicsScene->addActor(*box);

	PxParticleFluid* pf;

	//create particle system in PhysX SDK
	//set immutable properties
	PxU32 maxParticles = 4000;
	bool perParticleRestOffset = false;
	pf = g_Physics->createParticleFluid(maxParticles, perParticleRestOffset);

	pf->setRestParticleDistance(.3f);
	pf->setDynamicFriction(0.1f);
	pf->setStaticFriction(0.1f);
	pf->setDamping(0.1);
	pf->setParticleMass(.1);
	pf->setRestitution(0);
	pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, true);
	pf->setStiffness(1);

	if (pf)
	{
		g_PhysicsScene->addActor(*pf);
		//m_particleEmitter = new ParticleEmitter(maxParticles, PxVec3(0, 10, 0), pf, .01);
		//m_particleEmitter->setStartVelocityRange(-2.0f, 0, -2.0f, 2.0f, 0.0f, 2.0f);

		m_particleEmitter = new ParticleFluidEmitter(maxParticles, PxVec3(0, 10, 0), pf, .1);
		m_particleEmitter->setStartVelocityRange(-.001f, -250.0f, -0.001f, 0.001f, -250.0f, 0.001f);
	}

	//Player Controller
	myHitReport = new MyControllerHitReport();
	gCharacterManager = PxCreateControllerManager(*g_PhysicsScene);
	//describe our controller
	PxCapsuleControllerDesc desc;
	desc.height = 1.6f;
	desc.radius = 0.4f;
	desc.position.set(0, 0, 0);
	desc.material = g_PhysicsMaterial;
	desc.reportCallback = myHitReport; //connect it to our collision detection routine
	desc.density = 10;
	//create the layer controller
	gPlayerController = gCharacterManager->createController(desc);

	gPlayerController->setPosition(PxExtendedVec3(10, 10, 10));
	//set up some variables to control our player with
	_characterYVelocity = 0; //initialize character velocity
	_characterRotation = 0; //and rotation
	_playerGravity = -0.5f;
	myHitReport->ClearPlayerContactNormal(); //initialize the contact normal (what we are in contact with)
	gPlayerController->getActor()->setName("Player");
	gPlayerController->getActor()->userData = this;
	g_PhysicsScene->addActor(*gPlayerController->getActor()); //so we can draw its gizmo
	

}

