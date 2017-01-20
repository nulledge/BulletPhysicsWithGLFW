#ifndef __APPLICATION__
#define __APPLICATION__

#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "Application/WindowConfig.hpp"

namespace App {

enum LogType {
    Info, Warning, Error
};

class Application {

// Singleton pattern.
public:
    static Application* getInstance();
private:
    static Application* _instance;

// Constructor and distructor.
private:
    Application( void ) : _initialized( false ), _out( &std::cout ) {
        log( App::LogType::Info, "Application starts." );
        init();
    }
public:
    ~Application( void ) {
        log( App::LogType::Warning, "Application terminates." );
        if( _initialized == true ) {
            log( App::LogType::Warning, "GLFW terminates." );
            glfwTerminate();
        }
        log( App::LogType::Warning, "Program exits." );
        exit( EXIT_SUCCESS );
    }

public:
    const Application* hint( const int hint, const int value ) const;
    const Application* setLogStream( std::ostream& to );

private:
    void log( const LogType type, const std::string& info ) const;
    void init( void );

private:
    bool            _initialized;
    std::ostream*   _out;

};

// Default window config.
static const WindowConfig DEFAULT_CONFIG( 640U, 480U, "untitled" );

}

#endif