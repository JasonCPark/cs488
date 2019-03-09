#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>
#include "Texture.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;
	Texture myTexture;
	bool hasTexture = false;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{
	}

	void loadTexture(string filename) {
		myTexture = Texture();
		myTexture.loadPNG(filename);
		hasTexture = true;
	}


	~Triangle()
	{
		//delete myTexture;
	}
};

// A polygonal mesh.
class Mesh {
public:
  Mesh( const std::string& fname );
  
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
	void applyTrans(glm::mat4 trans);
};