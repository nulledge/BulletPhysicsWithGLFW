#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <iostream>
#include <cstdarg>

static const char* vertex_shader_text =
        "attribute vec4 in_position;                            "
        "void main()                                            "
        "{                                                      "
        "       gl_Position = in_position;                      "
        "}                                                      ";
static const char* fragment_shader_text =
        "uniform vec3 uniform_color;                            "
        "void main()                                            "
        "{                                                      "
        "       gl_FragColor = vec4 ( uniform_color, 1.0 );     "
        "}                                                      ";

static void error_callback( int error, const char* description )
{
        std::cout
                << "Error: " << description << std::endl;
}
static void key_callback( GLFWwindow* window,
        int key, int scancode, int action, int mods )
{
        if( ( key == GLFW_KEY_ESCAPE
                || key == GLFW_KEY_ENTER
                || key == GLFW_KEY_SPACE )
                && action == GLFW_PRESS )
                glfwSetWindowShouldClose( window, GLFW_TRUE );
        else
                std::cout
                        << "Warning: Not implemented." << std::endl;
}

GLuint LoadShader( GLenum type, const char* shaderCode ) {
        GLuint shader;
        GLint compiled;

        shader = glCreateShader( type );
        if( shader == 0U )
                return 0U;
        
        glShaderSource( shader, 1, &shaderCode, NULL );
        glCompileShader( shader );
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

        if( compiled == GLFW_FALSE ) {
                GLint infoLen = 0;

                glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLen );
                if( infoLen > 1 ) {
                        char* infoLog
                                = (char*)malloc( sizeof(char) * infoLen );

                        glGetShaderInfoLog(
                                shader,
                                infoLen,
                                NULL,
                                infoLog );
                        std::cout
                                << infoLog;

                        free( infoLog );
                }
                glDeleteShader( shader );
                return 0U;
        }
        return shader;
}

GLuint LinkProgram( GLuint vertexShader, GLuint fragmentShader ) {
        GLuint program;
        GLint linked;

        program = glCreateProgram();
        if( program == 0U ) {
                glfwTerminate();
                exit( EXIT_FAILURE );
        }
        glAttachShader( program, vertexShader );
        glAttachShader( program, fragmentShader );
        glLinkProgram( program );
        glGetProgramiv( program, GL_LINK_STATUS, &linked );

        if( linked == GLFW_FALSE ) {
                GLint infoLen = 0;
                
                glGetProgramiv( program, GL_INFO_LOG_LENGTH, &infoLen );
                if( infoLen > 1 ) {
                        char* infoLog 
                                = (char*)malloc( sizeof(char) * infoLen );

                        glGetProgramInfoLog(
                                program,
                                infoLen,
                                NULL,
                                infoLog );
                        std::cout
                                << infoLog;
                }
                glDeleteProgram( program );
                return 0U;
        }
        return program;
}

int main( void )
{
        // Default window config.
        const GLuint    WINDOW_WIDTH    = 640U;
        const GLuint    WINDOW_HEIGHT   = 480U;
        const char*     WINDOW_TITLE    = "untitled";
        GLFWmonitor*    FULLSCREEN      = NULL;
        GLFWwindow*     SHARE_CONTEXT   = NULL;

        // Set default error-callback.
        glfwSetErrorCallback( error_callback );

        // Initialize glfw.
        // IF SUCCESS, YOU MUST CALL glfwTerminate() BEFORE EXIT.
        if( glfwInit() == GLFW_FALSE )
                exit( EXIT_FAILURE ); // Initialize failed.

        // Client use openGL, neither openGL ES nor Vulkan.
        glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );

        // Create window.
        GLFWwindow* window = glfwCreateWindow(
                                WINDOW_WIDTH, WINDOW_HEIGHT,
                                WINDOW_TITLE,
                                FULLSCREEN, SHARE_CONTEXT
                                );
        if( window == NULL) { // Create failed.
                glfwTerminate();
                exit( EXIT_FAILURE );
        }
        glfwSetKeyCallback( window, key_callback ); // Bind key callback.
        
        // This thread have a control over the window.
        glfwMakeContextCurrent( window );
        gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );
        glfwSwapInterval( 1 );

        // Shader compile.
        GLuint vertex_shader
                = LoadShader( GL_VERTEX_SHADER, vertex_shader_text );
        GLuint fragment_shader
                = LoadShader( GL_FRAGMENT_SHADER, fragment_shader_text );

        // Program link.
        // openGL ES 3.0 require one and only one vertex and fragment shader.
        GLuint program = LinkProgram( vertex_shader, fragment_shader );

        // Run program.
        glUseProgram( program );

        // Triangle vertices to draw.
        const GLfloat vertices[] = {     0.0f,  0.5f, 0.0f,
                                        -0.5f, -0.5f, 0.0f,
                                         0.5f, -0.5f, 0.0f };
        // Triangle color to draw.
        const float     r = 0.0f,
                        g = 1.0f,
                        b = 1.0f;

        // Vertex shader attribute set.
        GLuint positionLocation = glGetAttribLocation( program, "in_position" );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray( positionLocation );

        // Fragment shader uniform set.
        GLuint colorLocation = glGetUniformLocation( program, "uniform_color" );
        glUniform3f( colorLocation, r, g, b );

        // Query all uniforms.
        GLint uniformLen, uniformNameLen;
        glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &uniformLen );
        glGetProgramiv( program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameLen );
        for( GLuint index = 0U; index < uniformLen; index += 1U ) {
                GLsizei length;
                GLint size;
                GLenum type;
                GLchar* name;
                glGetActiveUniform( program, index, uniformNameLen, &length, &size, &type, name );
        }

        // Run application.
        while( glfwWindowShouldClose( window ) == GLFW_FALSE ) {
                int width, height;
                glfwGetFramebufferSize( window, &width, &height );
                glViewport( 0, 0, width, height );
                glClear( GL_COLOR_BUFFER_BIT );

                glDrawArrays( GL_TRIANGLES, 0, 3 );
                glfwSwapBuffers( window );
                glfwPollEvents();
        }

        // Destroy unuse objects.
        glDeleteShader( vertex_shader );
        glDeleteShader( fragment_shader );
        glDeleteProgram( program );

        glfwDestroyWindow(window);
        glfwTerminate();
        exit( EXIT_SUCCESS );
}

/*#include <iostream>
#include <chrono>
#include <thread>

#include "btBulletDynamicsCommon.h"
#include "GLFW/glfw3.h"

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
}*/