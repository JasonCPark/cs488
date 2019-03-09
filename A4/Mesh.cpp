#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
	/*
	std::cout << "Constructor: Vertices" << std::endl;
	for (int i=0; i<m_vertices.size(); i++) {
		for (int j=0; j<3; j++) {
			std::cout << m_vertices[i][j] << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	*/
}

void Mesh::applyTrans(glm::mat4 trans) {
	/*
	std::cout << "Transformation Matrix" << std::endl;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			std::cout << trans[j][i] << ", ";
		}
		std::cout << std::endl;
	}
		std::cout << std::endl;


	std::cout << "Vertices1" << std::endl;
	for (int i=0; i<m_vertices.size(); i++) {
		for (int j=0; j<3; j++) {
			std::cout << m_vertices[i][j] << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	*/

	for (int i=0; i<m_vertices.size(); i++) {
		glm::vec4 verts4 = glm::vec4(m_vertices[i], 1);
		verts4 = trans * verts4;
		m_vertices[i] = glm::vec3(verts4);
	}
	/*
	std::cout << "Vertices2" << std::endl;
	for (int i=0; i<m_vertices.size(); i++) {
		for (int j=0; j<3; j++) {
			std::cout << m_vertices[i][j] << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	*/
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
