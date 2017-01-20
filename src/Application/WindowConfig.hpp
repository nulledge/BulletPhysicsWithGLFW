#ifndef __WINDOW_CONFIG__
#define __WINDOW_CONFIG__

#include <GLFW/glfw3.h>
#include <stdlib.h>

class WindowConfig {
public:
    WindowConfig( void ) : _fullscreen( NULL ), _sharedContext( NULL ) {}
    WindowConfig( const GLuint width, const GLuint height, const char* title )
    : WindowConfig() {
        setWidthAndHeight( width, height );
        setTitle( title );
    }

    const GLuint getWidth( void ) const;
    const GLuint getHeight( void ) const;
    const char* getTitle( void ) const;

    void setWidthAndHeight( const GLuint width, const GLuint height );
    void setWidth( const GLuint width );
    void setHeight( const GLuint height );
    void setTitle( const char* title );

private:
    GLuint          _width;
    GLuint          _height;
    char*           _title;
    GLFWmonitor*    _fullscreen;
    GLFWwindow*     _sharedContext;
};

#endif