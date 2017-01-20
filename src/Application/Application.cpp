#include "Application/Application.hpp"

#include <stdlib.h>

Application* Application::_instance = NULL;

Application* Application::getInstance() {
    if( Application::_instance == NULL )
        Application::_instance = new Application();
    return Application::_instance;
}