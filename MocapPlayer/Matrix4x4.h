#ifndef _Matrix4x4_h_
#define _Matrix4x4_h_

#include "Point3d.h"
#include "Vector3d.h"

/*================================================================================================================================================================*
 |	This is a 4x4 matrix class with methods that apply mainly to transformation matrices.                                                                         |
 *================================================================================================================================================================*/
class Matrix4x4{
private:
	double data[16];
public:
	/**
		default constructor
	*/
	Matrix4x4();

	/**
		destructor
	*/
	~Matrix4x4(void);

	/**
		copy constructor from another transformation matrix
	*/
	Matrix4x4(const Matrix4x4& other);

	/**
		copy operator from a transformation matrix
	*/
	Matrix4x4& operator = (const Matrix4x4& other);

	/**
		This method populates the data in the matrix with the values that are obtained from openGL (stored in column major order)
	*/
	void setOGLValues(const double* oglValues);

	/**
		This method populates the data in the matrix with the values that are passed in (stored in row major order)
	*/
	void setValues(const double* values);

	/**
		This method copies the data stored in the current matrix in the array of doubles provided as input.
	*/
	void getValues(double* values) const;

	/**
		This method copies the data stored in the current matrix in the array of doubles provided as input, in column major order.
	*/
	void getOGLValues(double* values)const;

	/**
		This method sets the current matrix to be equal to: a * b
	*/
	void setToProductOf(const Matrix4x4& a, const Matrix4x4& b);

	/**
		loads the identity matrix
	*/
	void loadIdentity();

	/**
		This method sets the current matrix to a 4x4 transformation matrix that corresponds to a translation
	*/
	void setToTranslationMatrix(const ThreeTuple& coords);

	/**
		This method sets the current matrix to a 4x4 transformation matrix that corresponds to a rotation of angle radians around the given axis.
		IT IS ASSUMED THAT THE VECTOR PASSED IN IS A UNIT VECTOR!!!
	*/
	void setToRotationMatrix(double angle, Vector3d& axis);

	/**
		This method returns a point that was transformed by the current matrix. Assume the point's w coordinate is 1!
	*/
	Point3d operator*(const Point3d &p) const;

	/**
		This method returns a vector that was transformed by the current matrix. Note that since the w-coordinate of the vector is 0, it will be unnafected
		by the translation part of the transformation matrix.
	*/
	Vector3d operator*(const Vector3d &v) const;


	/**
		This method returns the vector that corresponds to the translation associated with the current matrix.
		This vector goes from the origin of the global frame to the origin of the local frame, where this matrix
		corresponds to the transfomation matrix from the local frame into the golbal frame. The vector returned
		is specified in global coordinates.
	*/
	Vector3d getTranslation() const;

	/**
		This method returns the vector from the origin of the global frame to the origin of the local frame, where this matrix
		corresponds to the transfomation matrix from the local frame into the golbal frame. The vector returned is specified
		in local coordinates. This is equivalent to returning a vector that is equal to: R^T * r. The difference between the
		vector returned here, and the one obtained by looking at the translation of the inverse transformation matrix is that
		this one points from the origin of the global frame to the local frame, whereas the other one points the opposite way.
	*/
	Vector3d getLocalCoordTranslation() const;

	/**
		This method sets the translation part of the transformation matrix to the one that is passed in as a parameter.
	*/
	void setTranslation(const Vector3d& t);

	/**
		This method sets the translation part of the transformation matrix to the one that is passed in as a parameter.
	*/
	void setTranslation(const Point3d& t);

	/**
		This method clears the translation portion of the matrix.
	*/
	void clearTranslation();

	/**
		This method sets the current matrix to be equal to its transpose.
	*/
	void setToTranspose();
};

#endif




