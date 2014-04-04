//////////////////////////////////////////////////////////////
//	FILE: graphicsMath.h
//  created:  Oct 12 2009 by M. van de Panne, UBC
//  modified: Feb 2013 by B. Dalstein, UBC
//////////////////////////////////////////////////////////////

#ifndef GRAPHICS_MATH_H
#define GRAPHICS_MATH_H

int nChoosek( int n, int k );

// 3D vector
class Vector3
{
public:
	// Constructor: init to zero (e.g.: "Vector3 v;")
	Vector3();

	// Constructor: init  to x,y,z (e.g.: "Vector3 v1(0,1.5,-1);" or "v2 = Vector3(0,1.5,-1);")
	Vector3(double x, double y, double z);

	// Set or get i-th coordinate  (e.g.: "v2[1] = v1[0] + 42.0;")
	double & operator[](int i);
	double operator[](int i) const;

	// Set to zero
	void zero();

	// Print
	void print(const char * name) const;

	// Vector addition and substraction
	Vector3 operator+(const Vector3 & a) const;
	Vector3 operator-(const Vector3 & a) const;
	Vector3 & operator+=(const Vector3 & a);
	Vector3 & operator-=(const Vector3 & a);

	// Scalar multiplication and division
	friend Vector3 operator*(double scalar, const Vector3 & vector);
	Vector3 operator*(double scalar) const;
	Vector3 operator/(double scalar) const;
	Vector3 & operator*=(double scalar);
	Vector3 & operator/=(double scalar);

	// Cross product
	Vector3 cross(const Vector3 & a) const;

	// Dot product
	double dot(const Vector3 & a) const;

	// Component-wise product (=Hadamard product)
	Vector3 cwiseProduct(const Vector3 & a) const;

	// Compute length (i.e., two norm)
	double length() const;

	// Return normalized vector. Return (1,0,0) if length was 0.
	Vector3 normalized()  const;

	// Normalize in-place. Return false if length was 0.
	bool normalize();

private:
	double v[3];
};

// 3D affine transformations
// (Or equivalently, 3D coordinate frame)
// Internally represented as a 4x4 matrix using homogeneous coordinates
class Affine3
{
public:
	// Constructor:  initialize to identity
	Affine3();

	// Constructor: initialize to given coordinate frame
	Affine3(const Vector3 & S, const Vector3 & T, const Vector3 & U, const Vector3 & P0);

	// Constructor: initialize to given coordinate frame
	Affine3(double val[]);

	// Set to identity
	void iden();

	// Apply transformation to a vector
	Vector3 operator*(const Vector3 & a) const;

	// Transformation composition
	Affine3 operator*(const Affine3 &a) const;

	// Scalar multiplication
	Affine3 operator*(double s) const;

	// Print matrix, with title line
	void print(const char *name);

	// Set to rotation matrix, axis k and angle th (in degrees)
	void rotkth(const Vector3 & k, double th);

	// Set to rotation matrix, defined by a quaternion
	void quat(double q[4]);

	// Get (k,theta) from a rotation matrix
	void getkth(Vector3 &k, double &th);

	// Set to translation matrix
	void translate(const Vector3 & t);

	// Get columns as Vector3
	Vector3 getcol(int j);
	Vector3 getS();
	Vector3 getT();
	Vector3 getU();
	Vector3 getP0();

	// Compute inverse tranformation
	Affine3 inverse() const;

	// Set or get (i,j)-th coordinate
	double & operator() (int i, int j);
	double operator() (int i, int j) const;

	// Set or get (i,j)-th coordinate via operator[][]
	class Row { public:  double & operator[] (int j);
				private: friend class Affine3; double * v; };
	Row operator[] (int i);
	class ConstRow { public:  double operator[] (int j);
				 private: friend class Affine3; double v[4]; };
	ConstRow operator[] (int i) const;

private:
	double m[4][4];
};

#endif
