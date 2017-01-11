#include <iostream>
#include <chrono>
#include <thread>

#include "btBulletDynamicsCommon.h"

const btScalar FRAMERATE( 60. );
const btScalar FIXED_TIME_STEP( btScalar( 1. ) / FRAMERATE );

int main (void)
{
        btBroadphaseInterface* broadphase = new btDbvtBroadphase();

        btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
        btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

        btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

        btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

        dynamicsWorld->setGravity(btVector3(0, -10, 0));


        btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

        btCollisionShape* fallShape = new btSphereShape(1);


        btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
        btRigidBody::btRigidBodyConstructionInfo
                groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
        btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
        dynamicsWorld->addRigidBody(groundRigidBody);


        btDefaultMotionState* fallMotionState =
                new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
        btScalar mass = 1;
        btVector3 fallInertia(0, 0, 0);
        fallShape->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
        btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
        dynamicsWorld->addRigidBody(fallRigidBody);

        const uint64_t programDuration = 10U; // Simulate for 10 sec.
        std::chrono::system_clock::time_point lastCallTime;
        for( uint64_t frame = 0; frame < programDuration * FRAMERATE; frame += 1 ) {

                auto startTime = std::chrono::system_clock::now();

                float_t         timePassedSinceLastCall;
                const btScalar  maxTimeSteps( 10 );

                if( frame == 0 ) {
                        timePassedSinceLastCall = 1. / FRAMERATE;
                } else {
                        auto deltaTime =
                                std::chrono::duration_cast< std::chrono::milliseconds >( startTime - lastCallTime );
                        timePassedSinceLastCall = deltaTime.count() / 1000. ;
                }

                lastCallTime = std::chrono::system_clock::now();

                dynamicsWorld->stepSimulation( timePassedSinceLastCall,
                                                maxTimeSteps,
                                                FIXED_TIME_STEP );

                btTransform trans;
                fallRigidBody->getMotionState()->getWorldTransform( trans );

                if( frame % 60U == 0 ) {
                        std::cout << "Frame( " << frame << " )" << std::endl
                                << "Sphere Position( " << trans.getOrigin().getX() << ", "
                                                        << trans.getOrigin().getY() << ", "
                                                        << trans.getOrigin().getZ() << " )" << std::endl << std::endl;
                }

                auto currentTime = std::chrono::system_clock::now();
                auto goalTime   = startTime + std::chrono::milliseconds( (long long)( 1000 / FRAMERATE ) );
                std::this_thread::sleep_for( std::chrono::duration_cast< std::chrono::milliseconds >( goalTime - currentTime) );
        }

        dynamicsWorld->removeRigidBody(fallRigidBody);
        delete fallRigidBody->getMotionState();
        delete fallRigidBody;

        dynamicsWorld->removeRigidBody(groundRigidBody);
        delete groundRigidBody->getMotionState();
        delete groundRigidBody;


        delete fallShape;

        delete groundShape;


        delete dynamicsWorld;
        delete solver;
        delete collisionConfiguration;
        delete dispatcher;
        delete broadphase;

        return 0;
}