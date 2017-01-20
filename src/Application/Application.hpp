#ifndef __APPLICATION__
#define __APPLICATION__

#include <GLFW/glfw3.h>

#include "Application/WindowConfig.hpp"

class Application {
public:
    static Application* getInstance();
private:
    Application( void ) {}

private:
    static Application* _instance;
};

// Default window config.
static const WindowConfig config( 640U, 480U, "untitled" );

#endif