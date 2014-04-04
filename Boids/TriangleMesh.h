/**
  * Author:  Boris Dalstein (dalboris@cs.ubc.ca ; www.dalboris.fr)
  * Date:    2014
  * Licence: GNU General Public License v3
  *
  * Plain simple triangle mesh class for OpenGL.  
  *
  * It reads the mesh from  an OBJ file. Alternatively, you can create
  * an empty mesh and set its vertices and triangles programatically.
  *
  * The mesh is stored via the public members:
  *  - vertices: a 3D position for each vertex
  *  - triangles: three vertex indices for each triangle
  * 
  * It draws it using glBegin(), glVertex(), glNormal() and glEnd().
  * Normals are automatically computed when drawing.
  *
  * Nothing is cached: vertices and triangles can be safely modified
  * whenever you want, and still be drawn correctly.
  *
  */

#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include <vector>
#include "GraphicsMath.h"
#include <iostream>
#include <string>

using namespace std;

class TriangleMesh
{
public:
	// Member variables
        string name;
	typedef Vector3 Vertex;
	typedef Vector3 Normal;
	struct Triangle { unsigned int a, b, c; };
	std::vector<Vertex> vertices;
	std::vector<Normal> normals;
	std::vector<Triangle> triangles;

	void computeNormals();
	void normalize(float newsize);

	enum MeshDrawStyle { WIRE, SOLID, SHADED };
	void draw(MeshDrawStyle style = SHADED);     // draws triangle mesh
	void print();    // print triangle mesh

	// Creates an empty triangle mesh
	TriangleMesh();

	// Creates a triangle mesh from OBJ file
	TriangleMesh(const char * filename);

	// Replace the current triangle mesh by reading an OBJ file.
	// Ignore all lines but v and f lines.
	// Convert n-gons to triangles.
	void readFromOBJ(const char * filename);
};

// Cycle through TriangleMesh::DrawStyle
TriangleMesh::MeshDrawStyle & operator++(TriangleMesh::MeshDrawStyle & style);

#endif // TRIANGLEMESH_H
