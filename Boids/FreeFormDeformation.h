///////////////////////////////////////////////////////////////////
// FILE:      FreeFormDeformation.h
// CONTAINS:  template code for CPSC 426, Feb 2014, Assignment 2
///////////////////////////////////////////////////////////////////

#ifndef FREE_FORM_DEFORMATION_H
#define FREE_FORM_DEFORMATION_H

#include "GraphicsMath.h"
	
// Define the positions of the 27 control points of a 
// 2x2x2 lattice for free-form deformation
class FFDControlPoints
{
public:
	// Create a uniform lattice spanning the cube [0,1]x[0,1]x[0,1]
	// Equivalent to FFDControlPoints( (1,0,0), (0,1,0), (0,0,1), (0,0,0));
	FFDControlPoints();
	void zero();    // sets all control point values to zero
	
	// Create a uniform lattice spanning the cube [0,1]x[0,1]x[0,1]
	// of the provided coordinate frame. The coordinate frame can be
	// provided directly  as a  Affine3 tranformation matrix,  or by
	// its basis vectors ux,uy,uz and its origin p
	FFDControlPoints(const Affine3 & frame);
	FFDControlPoints(const Vector3 & S, const Vector3 & T, const Vector3 & U,
					 const Vector3 & P0);
	
	// Get or set a control point. 
	// i,j,k are the indices of the control point, they belong in {0,1,2}.
	// Example usage:
	//   ControlPoints cp;
	//   cp(0,0,0) = Vector3(0.4,42.0,18.0); // set the "front-bottom-left" control point
	//   cp(2,0,1)[1] = 12.5; // set the y component of the (2,0,1) control point
	Vector3 & operator() (int i, int j, int k);
	Vector3 operator() (int i, int j, int k) const;

	// Draw using OpenGL methods
	enum DrawStyle { None, PointsUniform, Points, Lattice };
	void draw(DrawStyle style = Lattice);

	// Print values to console
	void print(const char *name);

private:
	Vector3 data[27];
};

// Cycle through FFDControlPoints::DrawStyle
FFDControlPoints::DrawStyle & operator++(FFDControlPoints::DrawStyle & style);

// Compute the linear interpolation between two FFD 2x2x2 lattices
FFDControlPoints linearInterpolation(double u, const FFDControlPoints & p1, const FFDControlPoints & p2);

// Catmul-Rom interpolation of lattices
FFDControlPoints CRInterpolation(double u, const FFDControlPoints & p0, const FFDControlPoints & p1,
                                 const FFDControlPoints & p2, const FFDControlPoints & p3);

// compute the sum of two sets of control points.  Used for adding offsets.
FFDControlPoints FFDadd(const FFDControlPoints &a, const FFDControlPoints &b) ;

// Convenient structure to store the FFD coordinates of a vertex
// If you would prefer to use
//   typedef Vector3 FFDCoords;
// then feel free to do so
struct FFDCoords
{
	double s, t, u;
	FFDCoords() : s(0), t(0), u(0) {}
	FFDCoords(double s, double t, double u) : s(s), t(t), u(u) {}
};

#endif
