#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	double vx, vy, vz;
	size_t s1, s2, s3, s4;
	int faceInd;
	string filename;

	std::ifstream ifs( fname.c_str() );
	while(!ifs.eof()) {
		std::string code;
		std::string curLine;
		getline(ifs,curLine);
		std::istringstream lineStream(curLine);
		lineStream >> code;
		if( code == "v" ) {
			lineStream >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			std::regex triangleRegex("^f [0-9]+ [0-9]+ [0-9]+ ?$");
			std::regex rectRegex("^f [0-9]+ [0-9]+ [0-9]+ [0-9]+ ?$");
			if (std::regex_match(curLine, triangleRegex)) {
				lineStream >> s1 >> s2 >> s3;
				m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
			} else if (std::regex_match(curLine, rectRegex)) {
				lineStream >> s1 >> s2 >> s3 >> s4;
				m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
				m_faces.push_back( Triangle( s1 - 1, s3 - 1, s4 - 1 ) );
			}
		} else if ( code == "t" ) {
			lineStream >> faceInd >> filename;
			m_faces[faceInd].loadTexture(filename);
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
