#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/ext.hpp"
#include "glm/gtc/quaternion.hpp"

#include "PhysicsObject.h"

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
	
	DIYPhysicsSetup1();

    return true;
}

void Physics::shutdown()
{
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

	physicsScene->Update(m_window, m_delta_time);

	

    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
    //Gizmos::draw(m_camera.proj, m_camera.view);

    m_renderer->RenderAndClear(m_camera.view_proj);

	physicsScene->AddGizmos();
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
    
    vec4 geo_color(1, 0, 0, 1);
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
	//SphereClass* newBall0;
	//newBall0 = new SphereClass(glm::vec3(14, 2, 1), glm::vec3(3, 0, 3), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	//physicsScene->AddActor(newBall0);
	
	SphereClass* newBall1;
	newBall1 = new SphereClass(glm::vec3(0, 4, 10), glm::vec3(0, 0, -3), 3.0f, 1, glm::vec4(1, 0, 0, 1), false);
	physicsScene->AddActor(newBall1);

	BoxClass* newBox00;
	newBox00 = new BoxClass(glm::vec3(3, 2.f, 1), glm::vec3(-4.f, 0, 0), 3.0f, 1.f, 1.f, 1.f, glm::vec4(1, 0, 0, 1), false);
	physicsScene->AddActor(newBox00);
	//
	//BoxClass* newBox11;
	//newBox11 = new BoxClass(glm::vec3(0.0f, 3.5, 0), glm::vec3(0, 0, 0), 3.0f, 1.f, 1.f, 2.f, glm::vec4(1, 0, 0, 1), false);
	//physicsScene->AddActor(newBox11);
	//
	//BoxClass* newBox12;
	//newBox12 = new BoxClass(glm::vec3(-3.0f, 2.2f, 0), glm::vec3(0.8f, 0, 0), 3.0f, 1.f, 1.f, 2.f, glm::vec4(1, 0, 0, 1), false);
	//physicsScene->AddActor(newBox12);

	
	

	//Floor
	PlaneClass* newPlane0;
	newPlane0 = new PlaneClass(glm::normalize(glm::vec3(0, 1, 0)), 0);
	physicsScene->AddActor(newPlane0);

	//Boundaries
	BoxClass* newBox0;
	newBox0 = new BoxClass(glm::vec3(20, 0, 0), glm::vec3(0, 0, 0), 10000, 2, 5, 40, glm::vec4(0), true);
	physicsScene->AddActor(newBox0);
	BoxClass* newBox1;
	newBox1 = new BoxClass(glm::vec3(-20, 0, 0), glm::vec3(0, 0, 0), 10000, 2, 5, 40, glm::vec4(0), true);
	physicsScene->AddActor(newBox1);
	BoxClass* newBox2;
	newBox2 = new BoxClass(glm::vec3(0, 0, 20), glm::vec3(0, 0, 0), 10000, 38, 5, 2, glm::vec4(0), true);
	physicsScene->AddActor(newBox2);
	BoxClass* newBox3;
	newBox3 = new BoxClass(glm::vec3(0, 0, -20), glm::vec3(0, 0, 0), 10000, 38, 5, 2, glm::vec4(0), true);
	physicsScene->AddActor(newBox3);


	//int numberBalls = 10;
	//SphereClass* ball1 = new SphereClass(glm::vec3(1, 7, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), true);
	//SphereClass* ball2 = new SphereClass(glm::vec3(1, 5, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), false);
	//SphereClass* ball3 = new SphereClass(glm::vec3(1, 3, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), false);
	//SphereClass* ball4 = new SphereClass(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), false);
	//SpringJoint* joint1 = new SpringJoint(ball1, ball2, 0.5f, 0.2f);
	//SpringJoint* joint2 = new SpringJoint(ball2, ball3, 0.5f, 0.2f);
	//SpringJoint* joint3 = new SpringJoint(ball3, ball4, 0.5f, 0.2f);
	//
	//physicsScene->AddActor(ball1);
	//physicsScene->AddActor(ball2);
	//physicsScene->AddActor(ball3);
	//physicsScene->AddActor(ball4);
	//physicsScene->AddActor(joint1);
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
	//int width = 4;
	//int height = 4;
	//
	//SphereClass* ball[4][4];
	//for (int i = 0; i < 4; ++i)
	//{
	//	for (int j = 0; j < 4; ++j)
	//	{
	//		ball[i][j] = new SphereClass(glm::vec3(2 * i, 2 * j + 2, 1), glm::vec3(0, 0, 0), 3.0f, 0.5, glm::vec4(1, 0, 0, 1), false);
	//		physicsScene->AddActor(ball[i][j]);
	//	}
	//}
	//
	//SpringJoint* joint[24];
	//int jointCount = 0;
	//for (int i = 0; i < 4; ++i)
	//{
	//	for (int j = 0; j < 3; ++j)
	//	{
	//		joint[jointCount] = new SpringJoint(ball[i][j], ball[i][j + 1], 0.5, 0.5);
	//		jointCount++;
	//	}
	//}
	//for (int i = 0; i < 3; ++i)
	//{
	//	for (int j = 0; j < 4; ++j)
	//	{
	//		joint[jointCount] = new SpringJoint(ball[i][j], ball[i + 1][j], 0.5, 0.9); 
	//		jointCount++;
	//	}
	//}
	////physicsScene->AddActor(joint[jointCount]);
	//for (int i = 0; i < jointCount; ++i)
	//{
	//	physicsScene->AddActor(joint[i]);
	//}
}

