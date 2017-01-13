#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <iostream>
#include <cstdarg>

static const char* vertex_shader_text =
        "#version 330 core                                      "
        "in vec2 in_position;                                   "
        "in vec4 in_color;                                      "
        "out vec4 o_color;                                      "
        "void main()                                            "
        "{                                                      "
        "       gl_Position = vec4( in_position, 0.0, 1.0 );    "
        "       o_color = in_color;                             "
        "}                                                      ";
static const char* fragment_shader_text =
        "#version 330 core                                      "
        "in vec4 o_color;                                       "
        "layout( location = 0 ) out vec4 o_fragColor;           "
        "void main()                                            "
        "{                                                      "
        "       o_fragColor = vec4( o_color );                  "
        "}                                                      ";
static struct Vertex
{
        float x, y;
        float r, g, b, a;
} vertexes[] = {
                0.0f, 0.5f, 1.f, 0.f, 0.f, 1.f,
        -0.5f,-0.5f, 0.f, 1.f, 0.f, 1.f,
                0.5f,-0.5f, 0.f, 0.f, 1.f, 1.f
};

static void error_callback( int error, const char* description );
static void key_callback( GLFWwindow* window,
        int key, int scancode, int action, int mods );
static void resize_callback( GLFWwindow* window, int width, int height );

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
        glfwSetWindowSizeCallback( window, resize_callback );
        
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

        // Dissolve attribute location.
        GLuint posLoc = glGetAttribLocation( program, "in_position" );  // loc 0
        GLuint colLoc = glGetAttribLocation( program, "in_color" );     // loc 1

        GLuint VAOs[1];
        GLuint VBOs[1];

        // Create and bind VAO.
        glGenVertexArrays( 1, VAOs );
        glBindVertexArray( VAOs[ 0 ] );

        // Create and bind VBO.
        glGenBuffers( 1, VBOs );
        glBindBuffer( GL_ARRAY_BUFFER, VBOs[ 0 ] );
        glBufferData( GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW );

        // Mapping VBO and attribute location of in_position.
        glVertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE,
                sizeof(Vertex), (GLvoid*) 0 );
        glEnableVertexAttribArray( posLoc );

        // Mapping VBO and attribute location of in_color.
        glVertexAttribPointer( colLoc, 4, GL_FLOAT, GL_FALSE,
                sizeof(Vertex), (GLvoid*)( sizeof(GL_FLOAT) * 2 ) );
        glEnableVertexAttribArray( colLoc );

        // Detach VBO and VAO.
        glBindBuffer( GL_ARRAY_BUFFER, NULL );
        glBindVertexArray( NULL );

        // Run application.
        while( glfwWindowShouldClose( window ) == GLFW_FALSE ) {
                int width, height;
                glfwGetFramebufferSize( window, &width, &height );
                glViewport( 0, 0, width, height );
                glClearColor( CLEAR_COLOR );
                glClear( GL_COLOR_BUFFER_BIT );

                // Draw here.
                //=============================================================
                glBindVertexArray( VAOs[ 0 ] );
                glDrawArrays( GL_TRIANGLES, 0, 3 );
                glBindVertexArray( NULL );
                //=============================================================
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

static void resize_callback( GLFWwindow* window, int width, int height ) {
        std::cout
                << "Log: Resized " << width << "*" << height << std::endl;
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