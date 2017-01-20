#include "WindowConfig.hpp"

#include <stdlib.h>
#include <iostream>


const GLuint WindowConfig::getWidth( void ) const {
    return _width;
}
const GLuint WindowConfig::getHeight( void ) const {
    return _height;
}
const char* WindowConfig::getTitle( void ) const {
    return _title;
}

void WindowConfig::setWidthAndHeight( const GLuint width, const GLuint height ) {
    setWidth( width );
    setHeight( height );
}
void WindowConfig::setWidth( const GLuint width ) {
    _width = width;
}
void WindowConfig::setHeight( const GLuint height ) {
    _height = height;
}
void WindowConfig::setTitle( const char* title ) {
    delete _title;
    _title = (char*)malloc( strlen(title) * sizeof(char) );
    strcpy( _title, title );
}