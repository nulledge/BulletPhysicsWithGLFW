#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>

#include "util.h"

char* vertex_shader_text;
char* fragment_shader_text;

static void error_callback( int error, const char* description );
static void key_callback( GLFWwindow* window,
        int key, int scancode, int action, int mods );

void LoadShaderCode( char** out_shaderCode, unsigned int* out_length, const char* in_fileName );
GLuint LoadShader( GLenum type, const char* shaderCode );
GLuint LinkProgram( GLuint vertexShader, GLuint fragmentShader );

#define CLEAR_COLOR     1.f, 1.f, 1.f, 1.f

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
        // mac OS supports openGL API version 3.2 and above.
        // Specify client API version as 3.2.
        // However GLFW_CONTEXT_VERSION_MAJOR and GLFW_CONTEXT_VERSION_MINOR are
        // not hard contraints, API version above 3.2 could be selected.
        // A creation will fail if version below 3.2 is selected.
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
        // mas OS supports ONLY forward-compatible core profile context
        // for API version 3.2 and above.
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

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
        // Bind callbacks.
        glfwSetKeyCallback( window, key_callback );
        
        // This thread have a control over the window.
        glfwMakeContextCurrent( window );
        gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );
        glfwSwapInterval( 1 );

        // Load shader code.
        unsigned int vertexShaderCodeLength = 0U, fragmentShaderCodeLength = 0U;
        LoadShaderCode( &vertex_shader_text, &vertexShaderCodeLength, "src/shader/vertex.shader" );
        LoadShaderCode( &fragment_shader_text, &fragmentShaderCodeLength, "src/shader/fragment.shader" );

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

        // Dissolve attribute location.
        GLuint posLoc = glGetAttribLocation( program, "in_position" );  // loc 0
        GLuint colLoc = glGetAttribLocation( program, "in_color" );     // loc 1

        GLuint VAOs[1];
        GLuint VBOs[1];

        // Create and bind VAO.
        glGenVertexArrays( 1, VAOs );
        glBindVertexArray( VAOs[ 0 ] );

        // Import mesh.
        Mesh mesh;
        const char* meshPath = "res/shape.obj";
        if( FileLoadMesh( meshPath, &mesh ) == false ) {
                std::cout << "Error: Parse error! " << meshPath << std::endl;
        }
        struct AVertex {
                float x, y, z;
                float r, g, b, a;
        };
        AVertex* verticies;
        unsigned int index = 0U, size = 0U;
        verticies = (AVertex*)malloc( sizeof(AVertex) * 3 * mesh.f.size() );
        for( std::vector<Face>::iterator begin = mesh.f.begin(); begin != mesh.f.end(); begin += 1 ) {
                Face face = *(begin);
                for( unsigned int i = 0; i < 3U; i += 1U, index += 1U ) {
                        verticies[ index ].x = mesh.v[ face.verticies[ i ].v - 1 ].x;
                        verticies[ index ].y = mesh.v[ face.verticies[ i ].v - 1 ].y;
                        verticies[ index ].z = mesh.v[ face.verticies[ i ].v - 1 ].z;
                        verticies[ index ].r = mesh.vn[ face.verticies[ i ].vn - 1 ].x;
                        verticies[ index ].g = mesh.vn[ face.verticies[ i ].vn - 1 ].y;
                        verticies[ index ].b = mesh.vn[ face.verticies[ i ].vn - 1 ].z;
                        verticies[ index ].a = 1.0f;
                }
        }

        // Create and bind VBO.
        glGenBuffers( 1, VBOs );
        glBindBuffer( GL_ARRAY_BUFFER, VBOs[ 0 ] );
        glBufferData( GL_ARRAY_BUFFER,
                sizeof(AVertex) * 3 * mesh.f.size(),
                verticies,
                GL_STATIC_DRAW );

        // Mapping VBO and attribute location of in_position.
        glVertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE,
                sizeof(AVertex), (GLvoid*) 0 );
        glEnableVertexAttribArray( posLoc );

        // Mapping VBO and attribute location of in_color.
        glVertexAttribPointer( colLoc, 4, GL_FLOAT, GL_FALSE,
                sizeof(AVertex), (GLvoid*)( sizeof(GL_FLOAT) * 3 ) );
        glEnableVertexAttribArray( colLoc );

        // Detach VAO.
        glBindVertexArray( NULL );

        // Run application.
        while( glfwWindowShouldClose( window ) == GLFW_FALSE ) {
                int width, height;
                glfwGetFramebufferSize( window, &width, &height );
                glViewport( 0, 0, width, height );
                glClearColor( CLEAR_COLOR );
                glEnable( GL_DEPTH_TEST );
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                // Draw here.
                //=============================================================
                glBindVertexArray( VAOs[ 0 ] );

                glDrawArrays( GL_TRIANGLES, 0, 3 * mesh.f.size() );
                
                glBindVertexArray( NULL );
                //=============================================================

                glfwSwapBuffers( window );
                glfwPollEvents();
        }

        // Destroy unuse objects.
        glDeleteBuffers( 1, VBOs );
        glDeleteVertexArrays( 1, VAOs );
        glDeleteShader( vertex_shader );
        glDeleteShader( fragment_shader );
        glDeleteProgram( program );

        glfwDestroyWindow(window);
        glfwTerminate();
        exit( EXIT_SUCCESS );
}


static void error_callback( int error, const char* description ) {
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


void LoadShaderCode( char** out_shaderCode, unsigned int* out_length, const char* in_fileName ) {
        if( FileExist( in_fileName ) == false ) {
                std::cout
                        << "Error: File not exist, " << in_fileName << std::endl;
                return;
        }
        std::ifstream file( in_fileName );
        std::string result, input;
        while( std::getline( file, input ) ) {
                result += input + "\n";
        }
        *out_length = result.length();
        *out_shaderCode = (char*) malloc( sizeof(char) * ( *out_length + 1) );
        std::strcpy( *out_shaderCode, result.c_str() );
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