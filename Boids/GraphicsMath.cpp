/////////////////////////////////////////////////////////////
//	FILE: graphicsMath.cpp
//  created:  Oct 12 2009 by M. van de Panne, UBC
//  modified: Feb 2013 by B. Dalstein, UBC
///////////////////////////////////////////////////////////////

#include "GraphicsMath.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>

// defining PI
const double PI = 3.141592653589793238463;

int nChoosek( int n, int k )
{
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;
    
    int result = n;
    for( int i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

// Constructor: init to zero (e.g.: "Vector3 v;")
Vector3::Vector3() { v[0]=v[1]=v[2]=0.0; }

// Constructor: init  to x,y,z (e.g.: "Vector3 v1(0,1.5,-1);" or "v2 = Vector3(0,1.5,-1);")
Vector3::Vector3(double x, double y, double z) { v[0]=x; v[1]=y; v[2]=z; }

// Set or get i-th coordinate  (e.g.: "v2[1] = v1[0] + 42.0;")
double & Vector3::operator[](int i) { return v[i]; }
double Vector3::operator[](int i) const { return v[i]; }

// Set to zero
void Vector3::zero() {  v[0]=v[1]=v[2]=0.0; }

// Print
void Vector3::print(const char *name) const { printf("%s:  %6.3f %6.3f %6.3f \n",name,v[0],v[1],v[2]); }

// Vector addition and substraction
Vector3 Vector3::operator+(const Vector3 & a) const {
	return Vector3( v[0]+a.v[0], v[1]+a.v[1], v[2]+a.v[2]); }
Vector3 Vector3::operator-(const Vector3 & a) const {
	return Vector3( v[0]-a.v[0], v[1]-a.v[1], v[2]-a.v[2]); }
Vector3 & Vector3::operator+=(const Vector3 & a) {
	v[0]+=a.v[0], v[1]+=a.v[1], v[2]+=a.v[2];
	return *this; }
Vector3 & Vector3::operator-=(const Vector3 & a) {
	v[0]-=a.v[0], v[1]-=a.v[1], v[2]-=a.v[2];
	return *this; }

// cross product
Vector3 Vector3::cross(const Vector3 & a) const {
	return Vector3(
		v[1]+a.v[2] - v[2]*a.v[1],
		v[2]*a.v[0] - v[0]*a.v[2],
		v[0]*a.v[1] - v[1]*a.v[0]
	);
}

// dot product
double Vector3::dot(const Vector3 & a) const { return v[0]*a.v[0] + v[1]*a.v[1] + v[2]*a.v[2]; }

// component-wise product (=Hadamard product)
Vector3 Vector3::cwiseProduct(const Vector3 & a) const {
	return Vector3( v[0]*a.v[0], v[1]*a.v[1], v[2]*a.v[2]); }

// Compute length (i.e., two norm)
double Vector3::length() const { return (double) sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]); }

// Return normalized vector. Return (1,0,0) if length was 0.
Vector3 Vector3::normalized()  const {
	double size=length();
	if (size>0)
		return (*this) / size;
	else
		return Vector3(1,0,0);
}

// Normalize in-place. Return false if length was 0.
bool Vector3::normalize() {
	double size=length();
	if (size>0)
	{
		(*this) /= size;
		return true;
	}
	else
		return false;
}





Vector3 operator*(double scalar, const Vector3 & vector)
{
	return Vector3(scalar*vector[0], scalar*vector[1], scalar*vector[2]);
}

Vector3 Vector3::operator*(double scalar) const
{
	return Vector3(scalar*v[0], scalar*v[1], scalar*v[2]);
}

Vector3 & Vector3::operator*=(double scalar)
{
	v[0] *= scalar;
	v[1] *= scalar;
	v[2] *= scalar;
	return *this;
}

Vector3 Vector3::operator/(double scalar) const
{
	return (*this) * (1/scalar);
}

Vector3 & Vector3::operator/=(double scalar)
{
	return (*this) *= (1/scalar);
}

Affine3::Affine3() {
	iden();
}

Affine3::Affine3(const Vector3 & i, const Vector3 & j, const Vector3 & k, const Vector3 & p) {
	iden();
	for (int r=0; r<3; r++) {
		m[r][0] = i[r];
		m[r][1] = j[r];
		m[r][2] = k[r];
		m[r][3] = p[r];
	}
}

Affine3::Affine3(double val[]) {
	iden();
	for (int r=0; r<3; r++) {
	  m[r][0] = val[r];
	  m[r][1] = val[3+r];
	  m[r][2] = val[6+r];
	  m[r][3] = val[9+r];
	}
}


void Affine3::iden() {
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			m[i][j] = ( i==j? 1 : 0);
		}
	}
}

Vector3 Affine3::operator*(const Vector3 & a) const {
	return Vector3(
		m[0][0]*a[0] + m[0][1]*a[1] + m[0][2]*a[2] + m[0][3],
		m[1][0]*a[0] + m[1][1]*a[1] + m[1][2]*a[2] + m[1][3],
		m[2][0]*a[0] + m[2][1]*a[1] + m[2][2]*a[2] + m[2][3]
	);
}

Affine3 Affine3::operator*(const Affine3 & a) const {
	Affine3 ans;
	for (int r=0; r<4; r++) {
		for (int c=0; c<4; c++) {
			ans.m[r][c] = 0.0;
			for (int n=0; n<4; n++)
				ans.m[r][c] += m[r][n]*a.m[n][c];
		}
	}
	return ans;
}

Affine3 Affine3::operator*(double s) const {
	Affine3 ans;
	for (int r=0; r<4; r++) {
		for (int c=0; c<4; c++) {
			ans.m[r][c] = m[r][c] * s;
		}
	}
	return ans;
}

void Affine3::print(const char *name) {
	printf("%s\n",name);
	for (int r=0; r<4; r++) {
		printf("%6.3f %6.3f %6.3f %6.3f\n", m[r][0], m[r][1], m[r][2], m[r][3]);
	}
}

void Affine3::rotkth(const Vector3 & k, double th)
{

	double cth, sth, vth;
	double kx, ky, kz;

	th *= PI/180.0f;
	cth = (double) cos(th);
    vth = 1.0f-cth;
	sth = (double) sin(th);
    kx = k[0];
    ky = k[1];
    kz = k[2];
	iden();
    m[0][0] = kx*kx*vth + cth;
    m[0][1] = ky*kx*vth - kz*sth;
    m[0][2] = kz*kx*vth + ky*sth;
    m[1][0] = kx*ky*vth + kz*sth;
    m[1][1] = ky*ky*vth + cth;
    m[1][2] = kz*ky*vth - kx*sth;
    m[2][0] = kx*kz*vth - ky*sth;
    m[2][1] = ky*kz*vth + kx*sth;
    m[2][2] = kz*kz*vth + cth;
}

void Affine3::translate(const Vector3 & t)
{
	iden();
    m[0][3] = t[0];
    m[1][3] = t[1];
    m[2][3] = t[2];
}

Vector3 Affine3::getcol(int j)
{
	return Vector3( m[0][j], m[1][j], m[2][j]);
}

Vector3 Affine3::getS() { return getcol(0); }
Vector3 Affine3::getT() { return getcol(1); }
Vector3 Affine3::getU() { return getcol(2); }
Vector3 Affine3::getP0() { return getcol(3); }

void Affine3::quat(double q[])
{
	double w,x,y,z;
	double xs,ys,zs,wx,wy,wz,xx,yy,zz,xy,yz,xz;
	
	w = q[0];
	x = q[1];
	y = q[2];
	z = q[3];

	xs = x + x;
	ys = y + y;
	zs = z + z;
	wx = w*xs;
	wy = w*ys;
	wz = w*zs;
	xx = x*xs;
	yy = y*ys;
	zz = z*zs;
	xy = x*ys;
	yz = y*zs;
	xz = x*zs;
	m[0][0] = 1.0f - (yy+zz);
	m[0][1] = xy-wz;
	m[0][2] = xz+wy;
	m[0][3] = 0.0;
	m[1][0] = xy + wz;
	m[1][1] = 1.0f - (xx+zz);
	m[1][2] = yz - wx;
	m[1][3] = 0.0;
	m[2][0] = xz-wy;
	m[2][1] = yz + wx;
	m[2][2] = 1.0f - (xx+yy);
	m[2][3] = 0.0;
	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 1.0f;
}

void Affine3::getkth(Vector3 &k, double &th)
{
	double sth, cth;
	double a1,a2,a3;

    cth = 0.5f*(m[0][0] + m[1][1] + m[2][2] - 1.0f);
    a1 = m[2][1] - m[1][2];
    a2 = m[0][2] - m[2][0];
    a3 = m[1][0] - m[0][1];
	sth = 0.5f* (double) sqrt(a1*a1 + a2*a2 + a3*a3);
    if (fabs(sth)<0.0001f) {
      k[0] = 1.0f;
      k[1] = 0.0f;
      k[2] = 0.0f;
      th = 0.0f;
      return;
    }
	th = (double) atan2(sth, cth)*180.0/PI;
    k[0] = (m[2][1] - m[1][2])/(2.0f*sth);
    k[1] = (m[0][2] - m[2][0])/(2.0f*sth);
    k[2] = (m[1][0] - m[0][1])/(2.0f*sth);
}


// code for inverse adapted from Eigen implementation (GPL licence)
inline double det3_helper (const Affine3 & matrix, int i1, int i2, int i3, int j1, int j2, int j3)
{
	return matrix[i1][j1] * (matrix[i2][j2] * matrix[i3][j3] - matrix[i2][j3] * matrix[i3][j2]);
}

template<int i, int j>
inline double cofactor_4x4(const Affine3 & matrix)
{
  enum {
    i1 = (i+1) % 4,
    i2 = (i+2) % 4,
    i3 = (i+3) % 4,
    j1 = (j+1) % 4,
    j2 = (j+2) % 4,
    j3 = (j+3) % 4
  };
  return det3_helper(matrix, i1, i2, i3, j1, j2, j3)
       + det3_helper(matrix, i2, i3, i1, j1, j2, j3)
       + det3_helper(matrix, i3, i1, i2, j1, j2, j3);
}

Affine3 Affine3::inverse() const
{
	Affine3 result;
	result.m[0][0] =  cofactor_4x4<0,0>(*this);
	result.m[1][0] = -cofactor_4x4<0,1>(*this);
	result.m[2][0] =  cofactor_4x4<0,2>(*this);
	result.m[3][0] = -cofactor_4x4<0,3>(*this);
	result.m[0][2] =  cofactor_4x4<2,0>(*this);
	result.m[1][2] = -cofactor_4x4<2,1>(*this);
	result.m[2][2] =  cofactor_4x4<2,2>(*this);
	result.m[3][2] = -cofactor_4x4<2,3>(*this);
	result.m[0][1] = -cofactor_4x4<1,0>(*this);
	result.m[1][1] =  cofactor_4x4<1,1>(*this);
	result.m[2][1] = -cofactor_4x4<1,2>(*this);
	result.m[3][1] =  cofactor_4x4<1,3>(*this);
	result.m[0][3] = -cofactor_4x4<3,0>(*this);
	result.m[1][3] =  cofactor_4x4<3,1>(*this);
	result.m[2][3] = -cofactor_4x4<3,2>(*this);
	result.m[3][3] = cofactor_4x4<3,3>(*this);
	
	return result * (1/(
		m[0][0]*result.m[0][0] +
		m[1][0]*result.m[0][1] +
		m[2][0]*result.m[0][2] +
		m[3][0]*result.m[0][3]));
}

// Set or get (i,j)-th coordinate
double & Affine3::operator() (int i, int j) { return m[i][j]; }
double Affine3::operator() (int i, int j) const { return m[i][j]; }

// Set or get (i,j)-th coordinate via operator[][]
double & Affine3::Row::operator[] (int j)
{
	return v[j];
}
Affine3::Row Affine3::operator[] (int i)
{
	Row res;
	res.v = m[i];
	return res;
}

double Affine3::ConstRow::operator[] (int j)
{
	return v[j];
}

Affine3::ConstRow Affine3::operator[] (int i) const
{
	ConstRow res;
	res.v[0] = m[i][0];
	res.v[1] = m[i][1];
	res.v[2] = m[i][2];
	res.v[3] = m[i][3];
	return res;
}


///////////////////////////////////////////////////////////
// graphicsMathTest()
///////////////////////////////////////////////////////////

void graphicsMathTest() {
      // This routine is called from init() at program startup

	printf("\n Vector3 tests\n");
	Vector3 a(1,2,3);     //  create new vectors
	Vector3 b(1,1,1);    
	Vector3 c,e;
	a.print("a");
	b.print("b");
	double len1 = a.length();   // compute vector length, i.e., magnitude
	double len2 = b.length();
	double d = a.dot(b);     // dot product
	printf("len(a)=%6.3f, len(b)=%6.3f, dotprod=%6.3f\n", len1, len2, d );
	c = a + b;       // vector addition
	c.print("c=a+b");
	a += b;
	c.print("a+=b");
	a.normalize();   // vector normalisation
	a.print("a");
	printf("len(a)=%6.3f\n",a.length());
	a = Vector3(1,0,0);
	b = Vector3(0,1,0);
	c = a.cross(b);         // cross product
	printf("cross product of i x j:\n");
	c.print("c");
	e = a + b;
	e.print("e=a+b");
	e = a - b;
	e.print("e=a-b");

	printf("\n Affine3 tests\n");
	Affine3 f,g;
	f.print("matrix f");
	  // setup matrix
	  //     [ 0 -1  0 -2 ]
	  // f = [ 1  0  0  1 ]
	  //     [ 0  0  1  0 ]
	  //     [ 0  0  0  1 ]
	f = Affine3(           // matrix that does    Trans(-2,1,0)Rot(z,90)
		   Vector3(0,1,0),    // i
		   Vector3(-1,0,0),   // j
		   Vector3(0,0,1),    // k
		   Vector3(-2,1,0) ); // p  i.e., translation
	f.print("matrix f");
	Vector3 point(1,2,0);
	Vector3 pointPrime = f*point;       // p' = M*p   i.e., transform a point
	pointPrime.print("pointPrime");
	g = Affine3(           // matrix    Trans(-1,-2,0)Rot(z,-90)
		Vector3(0,-1,0),
		Vector3(1,0,0),
		Vector3(0,0,1),
		Vector3(-1,-2,0) );
	g.print("matrix g");
	Affine3 h = f*g;              // multiply two transformation matrices
	h.print("matrix h=f*g");
	g.rotkth(Vector3(0,0,1),90);     // g = Rot(z,90)
	g.print("Rot(z,90)");
	g.rotkth(Vector3(0,1,0),90);     // g = Rot(y,90)
	g.print("Rot(y,90)");
	Vector3 k;
	double theta;
	g.getkth(k,theta);            // compute (k,th) from a matrix
	k.print("k");
	printf("theta=%6.3f\n",theta);
}

