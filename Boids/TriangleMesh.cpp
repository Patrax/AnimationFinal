/**
  * Author:  Boris Dalstein (dalboris@cs.ubc.ca ; www.dalboris.fr)
  * Date:    2014
  * Licence: GNU General Public License v3
  *
  */

#include "TriangleMesh.h"

#include <cmath>
#include "defs.h"
// #include <GL/gl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

// Creates an empty triangle mesh
TriangleMesh::TriangleMesh() :
    vertices(),
    normals(),
    triangles()
{
}

// Creates a triangle mesh from OBJ file
TriangleMesh::TriangleMesh(const char * filename) :
    vertices(),
    normals(),
    triangles()
{
	readFromOBJ(filename);
}

// Replace the current triangle mesh by reading an OBJ file.
// Ignore all lines but v and f lines.
// Convert n-gons to triangles.
void TriangleMesh::readFromOBJ(const char * filename)
{
	using namespace std;

	// Clear mesh
	vertices.clear();
	normals.clear();
	triangles.clear();
	
	// Opening file
	string line;
	ifstream myfile(filename);
	name = filename;
	if (myfile.is_open())
	{
		while (getline(myfile,line))
		{
			// Split the line into words
			vector<string> words;
			string buf;
			stringstream ss(line);
			while (ss >> buf)
				words.push_back(buf);
			if(words.empty())
				continue;
			
			// Get line type
			string lineType = words[0];
			if(lineType == "v") // vertex
			{
				// convert to Vertex
				Vertex vertex(
					::atof(words[1].c_str()), // x coordinate
					::atof(words[2].c_str()), // y coordinate
					::atof(words[3].c_str()));// z coordinate
				
				// insert it in the list of vertices
				vertices.push_back(vertex);
			}
			else if(lineType == "f") // face
			{
				// number of vertices of the face
				int n = words.size()-1;
				
				// triangulate
				for(int i=0; i<n-2; i++)
				{
					// convert to Triangle (warning: obj starts at index 1)
					Triangle triangle;
					triangle.a = ::atoi(words[1].c_str()) - 1;
					triangle.b = ::atoi(words[i+2].c_str()) - 1;
					triangle.c = ::atoi(words[i+3].c_str()) - 1;
					
					// insert it in the list of triangles
					triangles.push_back(triangle);
				}
			}
			else
			{
				// line type not taken into account
				continue;
			}
		}
		myfile.close();
	}
	else 
	{
		cout << "Unable to open file: " << filename << endl; 
	}
}

//////////////////////////////////////////////////	
// Cycle through mesh rendering modes
//////////////////////////////////////////////////	

TriangleMesh::MeshDrawStyle & operator++(TriangleMesh::MeshDrawStyle & style)
{
	switch(style)	{
	case TriangleMesh::WIRE : return style = TriangleMesh::SOLID;
	case TriangleMesh::SOLID : return style = TriangleMesh::SHADED;
	case TriangleMesh::SHADED : return style = TriangleMesh::WIRE;
	}
}

//////////////////////////////////////////////////	
// print mesh data structure (if less than 50 vertices)
//////////////////////////////////////////////////	

void TriangleMesh::print()
{
  int nVert = vertices.size();
  int nTri = triangles.size();

  cout << "Mesh " << name << ": " << nVert << " vertices, " << nTri << " triangles" << endl;
  if (nVert>50) {
    cout << "Too many vertices to reasonably print." << endl;
    return;
  }
  for(int i=0; i<nVert; i++) 
    printf("vertex %d:  %6.3f %6.3f %6.3f\n", i, vertices[i][0], vertices[i][1], vertices[i][2]);
  for (int j=0; j<nTri; j++)
    printf("face %d:   %d %d %d\n", j, triangles[j].a, triangles[j].b, triangles[j].c); 
}

//////////////////////////////////////////////////	
// resize object to have unit length along the largest dimension
//////////////////////////////////////////////////	

void TriangleMesh::normalize(float newsize)
{
  Vector3 min, max, len;

  for (int d=0; d<3; d++) {
    min[d] = 1e10;
    max[d] = -1e10;
  }
  int n = vertices.size();
  for(int i=0; i<n; i++) {
    for (int d=0; d<3; d++) {
      float val = vertices[i][d];
      if (val>max[d]) 
	max[d] = val;
      if (val<min[d]) 
	min[d] = val;
    }
  }
  float maxlen = 0;
  for (int d=0; d<3; d++) {
    len[d] = max[d] - min[d];
    if (len[d] > maxlen)
      maxlen = len[d];
  }
  if (maxlen==0) return;
  float sf = newsize/maxlen;
  printf("maxlen=%f, sf=%f\n",maxlen,sf);
  for (int i=0; i<n; i++) {
    for (int d=0; d<3; d++) {
      float val = (vertices[i][d] - min[d])*sf;
      vertices[i][d] = val;
    }
  } 
}

//////////////////////////////////////////////////	
// Compute surface normals
//////////////////////////////////////////////////	

void TriangleMesh::computeNormals()
{
	int n = vertices.size();
	//	std::vector<Vertex> normals(n);
	normals.resize(n);
	for(int i=0; i<n; i++)
	{
		normals[i][0] = 0;
		normals[i][1] = 0;
		normals[i][2] = 0;
	}
	// Each incident face contributes
	for(int i=0; i<triangles.size(); i++)
	{
		// incident edges
		Vertex a = vertices[triangles[i].a];
		Vertex b = vertices[triangles[i].b];
		Vertex c = vertices[triangles[i].c];
		double ux = b[0]-a[0];
		double uy = b[1]-a[1];
		double uz = b[2]-a[2];
		double vx = c[0]-a[0];
		double vy = c[1]-a[1];
		double vz = c[2]-a[2];

		// cross-product
		double nx = - uy*vz + uz*vy;
		double ny = ux*vz - uz*vx;
		double nz = - ux*vy + uy*vx;
		double length = sqrt(nx*nx+ny*ny+nz*nz);
		if(length>0)
		{
			nx *= - 1 / length;
			ny *= - 1 / length;
			nz *= - 1 / length;
		}

		// contribute to vertices' normal
		normals[triangles[i].a][0] += nx;	normals[triangles[i].a][1] += ny;	normals[triangles[i].a][2] += nz;
		normals[triangles[i].b][0] += nx;	normals[triangles[i].b][1] += ny;	normals[triangles[i].b][2] += nz;
		normals[triangles[i].c][0] += nx;	normals[triangles[i].c][1] += ny;	normals[triangles[i].c][2] += nz;
	}
	// Normalize
	for(int i=0; i<n; i++)
	{
		Vertex v = normals[i];
		double length = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
		if(length>0)
		{
			normals[i][0] *= 1/length;
			normals[i][1] *= 1/length;
			normals[i][2] *= 1/length;
		}
	}
}

//////////////////////////////////////////////////	
// Draw mesh using deprecated OpenGL functions.
// To set a uniform color to the mesh, call glColor(...) beforehand
//////////////////////////////////////////////////	

void TriangleMesh::draw(MeshDrawStyle style)
{
	// ---------- Send geometry to OpenGL ----------

	switch(style) {
	case SHADED:
	  computeNormals();
	  glBegin(GL_TRIANGLES);
	  for(int i=0; i<triangles.size(); i++)
	    {
	      glNormal3d(normals[triangles[i].a][0], normals[triangles[i].a][1], normals[triangles[i].a][2]);
	      glVertex3d(vertices[triangles[i].a][0], vertices[triangles[i].a][1], vertices[triangles[i].a][2]);
	      glNormal3d(normals[triangles[i].b][0], normals[triangles[i].b][1], normals[triangles[i].b][2]);
	      glVertex3d(vertices[triangles[i].b][0], vertices[triangles[i].b][1], vertices[triangles[i].b][2]);
	      glNormal3d(normals[triangles[i].c][0], normals[triangles[i].c][1], normals[triangles[i].c][2]);
	      glVertex3d(vertices[triangles[i].c][0], vertices[triangles[i].c][1], vertices[triangles[i].c][2]);
	    }
	  glEnd();
	  break;
	case SOLID:
	  glDisable(GL_LIGHTING);
	  glColor3f(1.0, 1.0, 1.0);
	  glPolygonOffset(1.0, 2);
	  glEnable(GL_POLYGON_OFFSET_FILL);
	  glBegin(GL_TRIANGLES);
	  for(int i=0; i<triangles.size(); i++)
	    {
	      glVertex3d(vertices[triangles[i].a][0], vertices[triangles[i].a][1], vertices[triangles[i].a][2]);
	      glVertex3d(vertices[triangles[i].b][0], vertices[triangles[i].b][1], vertices[triangles[i].b][2]);
	      glVertex3d(vertices[triangles[i].c][0], vertices[triangles[i].c][1], vertices[triangles[i].c][2]);
	    }
	  glEnd();
	  glColor3f(0,0,0);
	  for(int i=0; i<triangles.size(); i++)
	    {
	      glBegin(GL_LINE_LOOP);
	      glVertex3d(vertices[triangles[i].a][0], vertices[triangles[i].a][1], vertices[triangles[i].a][2]);
	      glVertex3d(vertices[triangles[i].b][0], vertices[triangles[i].b][1], vertices[triangles[i].b][2]);
	      glVertex3d(vertices[triangles[i].c][0], vertices[triangles[i].c][1], vertices[triangles[i].c][2]);
	      glEnd();
	    }
	  glEnable(GL_LIGHTING);
	  glPolygonOffset(0,0);
	break;
	case WIRE:
	  glDisable(GL_LIGHTING);
	  glColor3f(1.0, 1.0, 1.0);
	  for(int i=0; i<triangles.size(); i++)
	    {
	      glBegin(GL_LINE_LOOP);
	      glVertex3d(vertices[triangles[i].a][0], vertices[triangles[i].a][1], vertices[triangles[i].a][2]);
	      glVertex3d(vertices[triangles[i].b][0], vertices[triangles[i].b][1], vertices[triangles[i].b][2]);
	      glVertex3d(vertices[triangles[i].c][0], vertices[triangles[i].c][1], vertices[triangles[i].c][2]);
	      glEnd();
	    }
	  glEnable(GL_LIGHTING);
	break;
	default:
	  break;
	}
}

