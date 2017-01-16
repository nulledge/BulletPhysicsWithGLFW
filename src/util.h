#ifndef __UTIL__
#define __UTIL__

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>

struct Vector3f {
    float x, y, z;
};
struct Vertex {
    unsigned int v, vn, vt;
};
struct Face {
    Vertex verticies[3];
};

struct Mesh {
    std::vector< Vector3f > v;
    std::vector< Vector3f > vn;
    std::vector< Vector3f > vt;
    std::vector< Face > f;
};

static bool FileExist( const char* in_fileName ) {
        std::ifstream file( in_fileName );
        return file.good();
}
enum ObjectFormat {
    v, vn, vt, f, comment, unknown
};
static ObjectFormat FormatResolve( std::string const& str ) {
    if( str == "v" ) return ::v;
    else if( str == "vn" ) return ::vn;
    else if( str == "vt" ) return ::vt;
    else if( str == "f" ) return ::f;
    else if( str == "#" ) return ::comment;
    else return ::unknown;
}
static bool FileLoadMesh( const char* in_fileName, Mesh* out_mesh ) {
    if( FileExist( in_fileName ) == false ) {
        return false;
    }
    std::ifstream file( in_fileName );
    std::string input;
    while( std::getline( file, input ) ) {
        std::istringstream iss( input );
        std::string type;

        iss >> type;

        switch( FormatResolve( type ) ) {
            case ::v : {
                std::string x, y, z;
                iss >> x >> y >> z;
                Vector3f vertex;
                vertex.x = atof( x.c_str() );
                vertex.y = atof( y.c_str() );
                vertex.z = atof( z.c_str() );
                out_mesh->v.push_back( vertex );
            }
            break;

            case ::vn : {
                std::string x, y, z;
                iss >> x >> y >> z;
                Vector3f vertexNormal;
                vertexNormal.x = atof( x.c_str() );
                vertexNormal.y = atof( y.c_str() );
                vertexNormal.z = atof( z.c_str() );
                out_mesh->vn.push_back( vertexNormal );
            }
            break;

            case ::vt : {
                std::string x, y, z;
                iss >> x >> y;
                if( iss )
                    iss >> z;
                else
                    z = "0";
                iss >> x >> y >> z;
                Vector3f vertexTexture;
                vertexTexture.x = atof( x.c_str() );
                vertexTexture.y = atof( y.c_str() );
                vertexTexture.z = atof( z.c_str() );
                out_mesh->vt.push_back( vertexTexture );
            }
            break;

            case ::f : {
                Face face;
                unsigned int* ref = reinterpret_cast<unsigned int*>( &face );
                unsigned int w = 0U;
                while( iss ) {
                    std::string vertex;
                    std::string delimiter = "/";
                    size_t pos = 0;
                    std::string token;
                    unsigned int ww = 0U;

                    iss >> vertex;
                    while( (pos = vertex.find( delimiter )) != std::string::npos ) {
                        token = vertex.substr( 0, pos );
                        vertex.erase( 0, pos + delimiter.length() );
                        ref[ 3*w + ww ] = (unsigned int)atoi( token.c_str() );
                        ww += 1U;
                    }
                    ref[ 3*w + ww ] = (unsigned int)atoi( vertex.c_str() );
                    w += 1U;
                }
                out_mesh->f.push_back( face );
            }
            break;

            case ::comment :
            break;

            case ::unknown :
            break;
        }
    }
    return true;
}

#endif