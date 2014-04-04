///////////////////////////////////////////////////////////////////
// FILE:      FreeFormDeformation.cpp
// CONTAINS:  template code for CPSC 426, Feb 2014, Assignment 2
///////////////////////////////////////////////////////////////////

#include "FreeFormDeformation.h"
#include "defs.h"
// #include <iostream>

// Creates   a   uniform   lattice   spanning  the   unit   cube
// [-1,1]x[-1,1]x[-1,1]
// Equivalent to ControlPoints( (1,0,0), (0,1,0), (0,0,1), (0,0,0));
FFDControlPoints::FFDControlPoints()
{
  zero();
}
	
void FFDControlPoints::zero()
{
  for (int n=0; n<27; n++) {
    data[n] = Vector3(0,0,0);
  }
}
	
// Creates a uniform lattice spanning the unit cube of the given
// coordinate frame
FFDControlPoints::FFDControlPoints(const Affine3 & frame)
{ 
	double coordFromIndex [3] = {0.0, 0.5, 1.0};
	
	for(int i=0; i<3; ++i)
	for(int j=0; j<3; ++j)
	for(int k=0; k<3; ++k)
	{
		data[i*9+j*3+k]= frame * Vector3(coordFromIndex[i],
							   coordFromIndex[j],
							   coordFromIndex[k]);
	}
}

FFDControlPoints::FFDControlPoints(const Vector3 & i,
				     const Vector3 & j, 
				     const Vector3 & k, 
				     const Vector3 & p)
{
	*this = FFDControlPoints(Affine3(i,j,k,p));
}

// Draw using OpenGL methods
void FFDControlPoints::draw(DrawStyle style)
{
	if (style == None)
		return;

	double red [3] = { 1.0, 0.0, 0.0 };
	double green [3] = { 0.0, 1.0, 0.0 };
	double blue [3] = { 0.0, 0.0, 1.0 };

	if(style == PointsUniform)
	{
		for(int i=0; i<3; ++i)
		{
			green[i] = red[i];
			blue[i] = red[i];
		}
	}

	glPointSize(3.0);
	glDisable(GL_LIGHTING);

	switch(style)
	{
	case None:
		break;

	case Lattice:
		glBegin(GL_LINES);
		// horizontal planes
		for(int j=0; j<3; ++j)
		{
			if(j==0) glColor3dv(red);
			else if(j==1) glColor3dv(green);
			else glColor3dv(blue);
			for(int i=0; i<3; ++i)
				for(int k=0; k<2; ++k)
				{
					glVertex3d((*this)(i,j,k)[0],(*this)(i,j,k)[1],(*this)(i,j,k)[2]);
					glVertex3d((*this)(i,j,k+1)[0],(*this)(i,j,k+1)[1],(*this)(i,j,k+1)[2]);
				}
			for(int k=0; k<3; ++k)
				for(int i=0; i<2; ++i)
				{
					glVertex3d((*this)(i,j,k)[0],(*this)(i,j,k)[1],(*this)(i,j,k)[2]);
					glVertex3d((*this)(i+1,j,k)[0],(*this)(i+1,j,k)[1],(*this)(i+1,j,k)[2]);
				}
		}
		// vertical lines
		for(int i=0; i<3; ++i)
		for(int k=0; k<3; ++k)
		{
			glColor3dv(red);
			glVertex3d((*this)(i,0,k)[0],(*this)(i,0,k)[1],(*this)(i,0,k)[2]);
			glColor3dv(green);
			glVertex3d((*this)(i,1,k)[0],(*this)(i,1,k)[1],(*this)(i,1,k)[2]);
			glColor3dv(green);
			glVertex3d((*this)(i,1,k)[0],(*this)(i,1,k)[1],(*this)(i,1,k)[2]);
			glColor3dv(blue);
			glVertex3d((*this)(i,2,k)[0],(*this)(i,2,k)[1],(*this)(i,2,k)[2]);
		}

		glEnd();

		// dont break on purpose

	case PointsUniform:
	case Points:
		glBegin(GL_POINTS);
		for(int j=0; j<3; ++j)
		{
			if(j==0) glColor3dv(red);
			else if(j==1) glColor3dv(green);
			else glColor3dv(blue);
			for(int i=0; i<3; ++i)
				for(int k=0; k<3; ++k)
					glVertex3d(data[i*9+j*3+k][0],data[i*9+j*3+k][1],data[i*9+j*3+k][2]);
		}
		glEnd();

	}

	glEnable(GL_LIGHTING);

}

// Print values to console
void FFDControlPoints::print(const char *name)
{
	std::cout << name << " : " << std::endl;
	for(int i=0; i<3; ++i)
	for(int j=0; j<3; ++j)
	for(int k=0; k<3; ++k)
	{
		std::cout << "  (" << i << "," << j << "," << k << ")";
		(*this)(i,j,k).print("");
	}
	std::cout << std::endl;
}

// Get or set a control point. 
// i,j,k belongs in {0,1,2}.
// Example usage:
//   ControlPoints cp;
//   cp(0,0,0) = Vector3(0.4,42.0,18.0); // set the "front-bottom-left" control point
//   cp(2,0,1)[1] = 12.5; // set the y component of the (2,0,1) control point
Vector3 & FFDControlPoints::operator() (int i, int j, int k)
{
	return data[i*9+j*3+k];
}
Vector3 FFDControlPoints::operator() (int i, int j, int k) const
{
	return data[i*9+j*3+k];
}

// Cycle through FFDControlPoints::DrawStyle
FFDControlPoints::DrawStyle & operator++(FFDControlPoints::DrawStyle & style)
{
	switch(style)
	{
	case FFDControlPoints::None : return style = FFDControlPoints::PointsUniform;
	case FFDControlPoints::PointsUniform : return style = FFDControlPoints::Points;
	case FFDControlPoints::Points : return style = FFDControlPoints::Lattice;
	case FFDControlPoints::Lattice : return style = FFDControlPoints::None;
	}
}

// Linear interpolation of lattices
FFDControlPoints linearInterpolation(double u, const FFDControlPoints & p1, const FFDControlPoints & p2)
{
	FFDControlPoints res;
	for(int i=0; i<3; ++i)
	for(int j=0; j<3; ++j)
	for(int k=0; k<3; ++k)
	{
		res(i,j,k) = (1-u)*p1(i,j,k) + u*p2(i,j,k);
	}
	return res;
}

Vector3 multiply(Vector3 p, float a){                 // Multiplication by a
    return Vector3(p[0]*a, p[1]*a, p[2]*a);
}

Vector3 divide(Vector3 p, float a){                 // Division by a
    return Vector3(p[0]/a, p[1]/a, p[2]/a);
}

Vector3 crTangent(Vector3 pPlusOne, Vector3 pMinusOne){
    return divide( (pPlusOne - pMinusOne) , 2 );
}


// Catmul-Rom interpolation of lattices
FFDControlPoints CRInterpolation(double u, const FFDControlPoints & p0, const FFDControlPoints & p1,
                                     const FFDControlPoints & p2, const FFDControlPoints & p3)
{
	FFDControlPoints res;
	for(int i=0; i<3; ++i)
    for(int j=0; j<3; ++j)
    for(int k=0; k<3; ++k)
    {
        res(i,j,k) = multiply((  multiply(p1(i,j,k), 2) +
                     multiply((p2(i,j,k)-p0(i,j,k)), u) +
                     multiply((multiply(p0(i,j,k), 2) - multiply(p1(i,j,k), 5) +
                     multiply(p2(i,j,k), 4) - p3(i,j,k)), u*u) +
                     multiply((multiply(p1(i,j,k), 3)- multiply(p2(i,j,k), 3) -
                     p0(i,j,k) + p3(i,j,k)), u*u*u)), 0.5);
    }
	return res;
}

FFDControlPoints FFDadd(const FFDControlPoints &a, const FFDControlPoints &b)
{
  FFDControlPoints res;
  for(int i=0; i<3; ++i)
  for(int j=0; j<3; ++j)
  for(int k=0; k<3; ++k)
    {
	  res(i,j,k) = a(i,j,k) + b(i,j,k);
    }
  return res;
}

