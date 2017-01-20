#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>
#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Application.hpp"
#include "util.h"

char* vertex_shader_text;
char* fragment_shader_text;

static void error_callback( int error, const char* description );
static void key_callback( GLFWwindow* window,
        int key, int scancode, int action, int mods );

void LoadShaderCode( char** out_shaderCode, unsigned int* out_length, const char* in_fileName );
GLuint LoadShader( GLenum type, const char* shaderCode );
GLuint LinkProgram( GLuint vertexShader, GLuint fragmentShader );

#define CLEAR_COLOR     0.f, 0.f, 0.f, 1.f

int main( void )
{
        App::Application* app = App::Application::getInstance();

        /* Set default error-callback. */
        glfwSetErrorCallback( error_callback );

        /* Client use openGL, neither openGL ES nor Vulkan. */
        app->hint( GLFW_CLIENT_API, GLFW_OPENGL_API )
        /*
        mac OS supports openGL API version 3.2 and above.
        Specify client API version as 3.2.
        However GLFW_CONTEXT_VERSION_MAJOR and GLFW_CONTEXT_VERSION_MINOR are
        not hard contraints, API version above 3.2 could be selected.
        A creation will fail if version below 3.2 is selected.
        */
                ->hint( GLFW_CONTEXT_VERSION_MAJOR, 3 )
                ->hint( GLFW_CONTEXT_VERSION_MINOR, 2 )
        /*
        mas OS supports ONLY forward-compatible core profile context
        for API version 3.2 and above.
        */
                ->hint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE )
                ->hint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

        // Create window.
        // GLFWwindow object encapsulates both a window and a context.
        // OpenGL rendering context(a.k.a. context) is a port all OpenGL
        // commands pass. It also includes states used for OpenGL. A Context is
        // required to call OpenGL APIs.
        GLFWwindow* window = glfwCreateWindow(
                App::DEFAULT_CONFIG.getWidth(), App::DEFAULT_CONFIG.getHeight(),
                App::DEFAULT_CONFIG.getTitle(),
                NULL, NULL );
        if( window == NULL) { // Create failed.
                glfwTerminate();
                exit( EXIT_FAILURE );
        }
        // Bind key callbacks.
        glfwSetKeyCallback( window, key_callback );
        
        // This thread have a control over the context. Only one context can be
        // current for a thread.
        glfwMakeContextCurrent( window );
        // Load pointers to openGL and its extension functions at runtime.
        // It is required above openGL 1.2.
        gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );
        // glfw supports a double-buffering.
        // A swap interval restricts buffer-swap. Even if the back-buffer is
        // filled and the graphic device is ready to swap buffer, do not swap
        // buffer immediately. A buffer-swap occures after the graphic device
        // draw all fragments from the front-buffer.
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

        // Import mesh.
        Mesh mesh;
        const char* meshPath = "res/pumpkin";
        if( FileLoadMesh( meshPath, &mesh ) == false ) {
                std::cout << "Error: Parse error! " << meshPath << std::endl;
        }

        // Convert struct Mesh to struct AVertex and AColor.
        struct AVertex {
                float x, y, z;
        } *verticies;
        struct AColor {
                float r, g, b, a;
        } *colors;
        unsigned int index = 0U, size = 0U;
        verticies = (AVertex*)malloc( sizeof(AVertex) * mesh.v.size() );
        colors = (AColor*)malloc( sizeof(AColor)* mesh.v.size() );
        for( unsigned int index = 0U; index < mesh.v.size(); index += 1U ) {
                verticies[ index ] = *reinterpret_cast<AVertex*>( &mesh.v[ index ] );
                colors[ index ].r = std::abs( mesh.v[ index ].x ) / 50.f;
                colors[ index ].g = std::abs( mesh.v[ index ].y ) / 50.f;
                colors[ index ].b = (std::abs( mesh.v[ index ].z ) - 100.f) / 50.f;
                colors[ index ].a = 1.f;
        }

        // Dissolve attribute location.
        GLuint posLoc = glGetAttribLocation( program, "in_position" );  // loc 0
        GLuint colLoc = glGetAttribLocation( program, "in_color" );     // loc 1
        GLuint mvpLoc = glGetUniformLocation( program, "in_mvp" );// loc 2

        GLuint VAOs[ 1 ];
        GLuint VBOs[ 5 ];
        GLuint VBOvertexPosition, VBOvertexColor, VBOelementArray,
                VBOuniformBlockPrefix, VBOuniformBlockSuffix;

        // Create and bind the vertex array object.
        glGenVertexArrays( 1, VAOs );
        glBindVertexArray( VAOs[ 0 ] );

        // Create four vertex buffer objects.
        glGenBuffers( 5, VBOs );
        VBOvertexPosition       = VBOs[ 0 ];
        VBOvertexColor          = VBOs[ 1 ];
        VBOelementArray         = VBOs[ 2 ];
        VBOuniformBlockPrefix   = VBOs[ 3 ];
        VBOuniformBlockSuffix   = VBOs[ 4 ];

        // Use vertex buffer objects as GL_ARRAY_BUFFER for a position.
        glBindBuffer( GL_ARRAY_BUFFER, VBOvertexPosition );
        glBufferData( GL_ARRAY_BUFFER,
                sizeof(AVertex) * mesh.v.size(),
                verticies,
                GL_STATIC_DRAW );
        // Mapping VBO and attribute location of in_position.
        glVertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE,
                sizeof(AVertex), (GLvoid*) 0 );
        glEnableVertexAttribArray( posLoc );

        // Use vertex buffer objects as GL_ARRAY_BUFFER for a color.
        glBindBuffer( GL_ARRAY_BUFFER, VBOvertexColor );
        glBufferData( GL_ARRAY_BUFFER,
                sizeof(AColor) * mesh.v.size(),
                colors,
                GL_STATIC_DRAW );
        // Mapping VBO and attribute location of in_color.
        glVertexAttribPointer( colLoc, 4, GL_FLOAT, GL_FALSE,
                sizeof(AColor), (GLvoid*) 0 );
        glEnableVertexAttribArray( colLoc );

        // Bind buffer to NULL is ignored, but just call it.
        glBindBuffer( GL_ARRAY_BUFFER, NULL );

        // Bind multiple uniform buffers.
        const float prefixColor[ 2 ] = { 1.f, 1.f },    // r, g
                suffixColor[ 2 ] = { 1.f, 1.f };        // b, a
        GLuint prefixBindPoint = 1U,    // Allocate binding point 1.
                suffixBindPoint = 2U;   // Allocate binding point 2.
        GLuint prefixBlockLoc = glGetUniformBlockIndex( program, "ColorPrefix" ),
                suffixBlockLoc = glGetUniformBlockIndex( program, "ColorSuffix" );
        glUniformBlockBinding( program, prefixBlockLoc, prefixBindPoint );
        glUniformBlockBinding( program, suffixBlockLoc, suffixBindPoint );

        glBindBuffer( GL_UNIFORM_BUFFER, VBOuniformBlockPrefix );
        glBufferData( GL_UNIFORM_BUFFER, sizeof(GLfloat) * 2, prefixColor, GL_STATIC_DRAW );
        glBindBuffer( GL_UNIFORM_BUFFER, VBOuniformBlockSuffix );
        glBufferData( GL_UNIFORM_BUFFER, sizeof(GLfloat) * 2, suffixColor, GL_STATIC_DRAW );

        // Bind buffer to NULL is ignored, but just call it.
        glBindBuffer( GL_UNIFORM_BUFFER, NULL );

        glBindBufferBase( GL_UNIFORM_BUFFER, prefixBindPoint, VBOuniformBlockPrefix );
        glBindBufferBase( GL_UNIFORM_BUFFER, suffixBindPoint, VBOuniformBlockSuffix );

        // Bind index buffer;
        struct AIndex {
                unsigned int a, b, c;
        } *indicies ;
        indicies = (AIndex*)malloc( sizeof(AIndex) * mesh.f.size() );
        for( unsigned int index = 0U; index < mesh.f.size(); index += 1U ) {
                indicies[ index ].a = mesh.f[ index ].verticies[ 0 ].v - 1;
                indicies[ index ].b = mesh.f[ index ].verticies[ 1 ].v - 1;
                indicies[ index ].c = mesh.f[ index ].verticies[ 2 ].v - 1;
        }
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, VBOelementArray );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                sizeof(AIndex) * mesh.f.size(),
                indicies,
                GL_STATIC_DRAW );

        // Detach VAO.
        glBindVertexArray( NULL );

        // Run application.
        while( glfwWindowShouldClose( window ) == GLFW_FALSE ) {
                int width, height;
                glfwGetFramebufferSize( window, &width, &height );
                glViewport( 0, 0, width, height );
                glClearColor( CLEAR_COLOR );
                glEnable( GL_DEPTH_TEST );
                glEnable( GL_CULL_FACE );
                glCullFace( GL_BACK );
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                // Draw here.
                //=============================================================
                glBindVertexArray( VAOs[ 0 ] );

                glm::mat4 Projection = 
                        glm::perspectiveFov( glm::radians( 45.0f ), (float)width, (float)height, 0.1f, 100.f );
                glm::mat4 View = glm::lookAt(
                        glm::vec3( 0.f, 65.f, 30.f),    // camera center
                        glm::vec3( 0.f, 50.f, 0.f ),     // camera look at
                        glm::vec3( 0.f, 1.f, 0.f ) );   // camera up vector
                glm::mat4 Model =
                        glm::translate( glm::mat4( 1.f ), glm::vec3( 0.f, 50.f, 0.f ) )
                        * glm::rotate( glm::mat4( 1.f ), (float)glm::radians( glfwGetTime()*50 ), glm::vec3( 0.f, 1.f, 0.f ) )
                        * glm::rotate( glm::mat4( 1.f ), (float)glm::radians( -90.f), glm::vec3( 1.f, 0.f, 0.f ) )
                        * glm::scale( glm::mat4( 1.f ), glm::vec3( 0.2f, 0.2f, 0.2f ) )
                        * glm::translate( glm::mat4( 1.f ), glm::vec3( 0.f, 0.f, 100.f ) );
                glm::mat4 MVP = Projection * View * Model;
                glUniformMatrix4fv( mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP) );

                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                glDrawElements( GL_TRIANGLES, 3 * mesh.f.size(), GL_UNSIGNED_INT, 0 );
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                
                glBindVertexArray( NULL );
                //=============================================================

                glDisable( GL_CULL_FACE );
                glDisable( GL_DEPTH_TEST );
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

        delete app;
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