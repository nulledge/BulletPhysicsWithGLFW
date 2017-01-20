#include "Application/Application.hpp"

#include <sstream>

App::Application* App::Application::_instance = NULL;

App::Application* App::Application::getInstance() {
    if( App::Application::_instance == NULL )
        App::Application::_instance = new Application();
    return App::Application::_instance;
}

static const std::string resolveHint( const int hint ) {
    switch( hint ) {
        case GLFW_CLIENT_API:
            return std::string( "GLFW_CLIENT_API" );
        break;
        case GLFW_CONTEXT_VERSION_MAJOR:
            return std::string( "GLFW_CONTEXT_VERSION_MAJOR" );
        break;
        case GLFW_CONTEXT_VERSION_MINOR:
            return std::string( "GLFW_CONTEXT_VERSION_MINOR" );
        break;
        case GLFW_OPENGL_FORWARD_COMPAT:
            return std::string( "GLFW_OPENGL_FORWARD_COMPAT" );
        break;
        case GLFW_OPENGL_PROFILE:
            return std::string( "GLFW_OPENGL_PROFILE" );
        break;

        default:
            return std::to_string( hint );
    }
}
static const std::string resolveValue( const int hint, const int value ) {
    switch( hint ) {
        case GLFW_CLIENT_API:
            switch( value ) {
                case GLFW_OPENGL_API:
                    return std::string( "GLFW_OPENGL_API" );
                default:
                    return std::to_string( value );
            }
        case GLFW_CONTEXT_VERSION_MAJOR:
                return std::to_string( value );
        case GLFW_CONTEXT_VERSION_MINOR:
                return std::to_string( value );
        case GLFW_OPENGL_FORWARD_COMPAT:
            switch( value ) {
                case GL_TRUE:
                    return std::string( "GL_TRUE" );
                default:
                    return std::to_string( value );
            }
        case GLFW_OPENGL_PROFILE:
            switch( value ) {
                case GLFW_OPENGL_CORE_PROFILE:
                    return std::string( "GLFW_OPENGL_CORE_PROFILE" );
                default:
                    return std::to_string( value );
            }

        default:
            return std::to_string( value );
    }
}

const App::Application* App::Application::hint( const int hint,
    const int value ) const {
    if( _initialized == false )
        return NULL;
    glfwWindowHint( hint, value );
    std::stringbuf info;
    std::ostream stream( &info );
    stream << "Change hint " << resolveHint( hint )
        << " with " << resolveValue( hint, value ) << ".";
    App::Application::log( App::LogType::Info, info.str() );
    return this;
}

const App::Application* App::Application::setLogStream( std::ostream& to ) {
    App::Application::log( App::LogType::Info, "Changing log outstream." );
    _out = &to;
    App::Application::log( App::LogType::Info, "Changed log outstream." );
    return this;
}

void App::Application::init( void ) {
    /*
    Initialize glfw.
    IF SUCCESS, YOU MUST CALL glfwTerminate() BEFORE EXIT.
    */
    switch( glfwInit() ) {
        case GLFW_FALSE:
            App::Application::log( App::LogType::Error,
                "GLFW initialization failed." );
        break;
        case GLFW_TRUE:
            App::Application::log( App::LogType::Info,"GLFW initializes." );
            _initialized = true;
        break;
        default:
            App::Application::log( App::LogType::Error, "Unknown state." );
        break;
    }
    glfwDefaultWindowHints();
    log( App::LogType::Info, "GLFW window hint is set to default.");
}

void App::Application::log( const App::LogType type,
    const std::string& info ) const {
    switch( type ) {
        case App::LogType::Info :
        (*_out) << "Info: " << info << std::endl;
        break;;

        case App::LogType::Warning :
        (*_out) << "Warning: " << info << std::endl;
        break;

        case App::LogType::Error :
        (*_out) << "Error: " << info << std::endl;
        exit( EXIT_FAILURE );
        break;

        default :
        App::Application::log( App::LogType::Error, "Not implemented." );
        break;
    }
}