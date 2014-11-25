#pragma once

class Vector3;
class Vector4;

/*!
*	4x4 Matrix
*/
class Matrix4x4
{
public:
	f32 m[4][4];
	Matrix4x4();
	Matrix4x4(const Matrix4x4 &m);
	Matrix4x4 operator*(const Matrix4x4 &m) const;

	f32 &operator[](uint32 index) const;

	bool operator==(const Matrix4x4 &o) const;
	bool operator!=(const Matrix4x4 &o) const;

	/*!
	*	\return this matrix's Determinant
	*/
	f32 Determinant() const;

	/*!
	*	Resets this matrix back to an Identity Matrix
	*/
	void Identity();

	/*!
	*	Transposes this matrix (reverses rows and columns)
	*/
	void Transpose();

	/*!
	*	\return whether this matrix was successfully inverted
	*/
	bool Inverse();

	/*!
	*	\return a Scale Matrix
	*	\param Scales a Vector4 containing the scale factor for each axis (x, y, z, w)
	*/
	static Matrix4x4 Scale(Vector4 Scales);

	/*!
	*	\return a Rotation Matrix
	*	\param Angles a Vector3 of Angles in Radians
	*/
	static Matrix4x4 Rotate(Vector3 Angles);

	/*!
	*	\return a Translation Matrix
	*	\param v the position we want to translate to
	*	\note v.w should be different than 0 (preferably 1) if you want the position to be transformed
	*/
	static Matrix4x4 Translate(Vector4 v);

	/*!
	*	\return a Viewport matrix
	*	\param x the viewport's x position
	*	\param y the viewport's y position
	*	\param Width the viewport's width
	*	\param Height the viewport's height
	*/
	static Matrix4x4 Viewport(f32 x, f32 y, f32 Width, f32 Height);

	/*!
	*	\return a Left Handed Perspective Matrix
	*	\param FOV Field of View in radians
	*	\param AspectRatio the aspect ratio
	*	\param zNear is the viewport's zNear value
	*	\param zFar is the viewport's zFar value
	*/
	static Matrix4x4 PerspectiveMatrixLH(f32 FOV, f32 AspectRatio, f32 zNear, f32 zFar);

	/*!
	*	\return a Right Handed Perspective Matrix
	*	\param FOV Field of View in radians
	*	\param AspectRatio the aspect ratio
	*	\param zNear is the viewport's zNear value
	*	\param zFar is the viewport's zFar value
	*/
	static Matrix4x4 PerspectiveMatrixRH(f32 FOV, f32 AspectRatio, f32 zNear, f32 zFar);

	/*!
	*	\return a Left Handed Look-At Matrix
	*	\param Position where the camera is located
	*	\param EyePosition where the camera is looking
	*	\param UpVector which direction is up
	*	\note EyePosition is a position, not a direction
	*/
	static Matrix4x4 LookAtMatrixLH(Vector3 Position, Vector3 EyePosition, Vector3 UpVector);

	/*!
	*	\return a Right Handed Look-At Matrix
	*	\param Position where the camera is located
	*	\param EyePosition where the camera is looking
	*	\param UpVector which direction is up
	*	\note EyePosition is a position, not a direction
	*/
	static Matrix4x4 LookAtMatrixRH(Vector3 Position, Vector3 EyePosition, Vector3 UpVector);

	/*!
	*	\return a Left-Handed Ortho Matrix
	*
	*	\param Left is the minimum x-value
	*	\param Right is the maximum x-value
	*	\param Bottom is the minimum y-value
	*	\param Top is the maximum y-value
	*	\param zNear is the viewport's zNear value
	*	\param zFar is the viewport's zFar value
	*/
	static Matrix4x4 OrthoMatrixLH(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 zNear, f32 zFar);

	/*!
	*	\return a Right-Handed Ortho Matrix
	*
	*	\param Left is the minimum x-value
	*	\param Right is the maximum x-value
	*	\param Bottom is the minimum y-value
	*	\param Top is the maximum y-value
	*	\param zNear is the viewport's zNear value
	*	\param zFar is the viewport's zFar value
	*/
	static Matrix4x4 OrthoMatrixRH(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 zNear, f32 zFar);
};

/*!
*	2D Vector (Point) class
*	Uses x, y
*/
class Vector2
{
public:
	f32 x, y;
	Vector2();
	Vector2(f32 x, f32 y);
	Vector2(const Vector2 &o);

	Vector2 operator+(const Vector2 &o) const;
	Vector2 operator+(f32 f) const;

	Vector2 operator-(const Vector2 &o) const;
	Vector2 operator-(f32 f) const;

	Vector2 operator*(const Vector2 &o) const;
	Vector2 operator*(f32 f) const;

	Vector2 operator/(const Vector2 &o) const;
	Vector2 operator/(f32 f) const;

	void operator+=(const Vector2 &o);
	void operator+=(f32 f);

	void operator-=(const Vector2 &o);
	void operator-=(f32 f);

	void operator/=(const Vector2 &o);
	void operator/=(f32 f);

	void operator*=(const Vector2 &o);
	void operator*=(f32 f);

	Vector2 operator-() const;

	bool operator==(const Vector2 &o) const;
	bool operator!=(const Vector2 &o) const;

	bool operator<(const Vector2 &o) const;
	bool operator>(const Vector2 &o) const;
	bool operator<=(const Vector2 &o) const;
	bool operator>=(const Vector2 &o) const;

	/*!
	*	\return the magnitude (length) of this vector
	*/
	f32 Magnitude() const;

	/*!
	*	\return the magnitude (length) of this vector
	*	\note unlike Magnitude, this does not use the square root to get the actual accurate magnitude
	*/
	f32 MagnitudeSquare() const;

	/*!
	*	\return a Normalized copy of this vector
	*/
	Vector2 Normalized() const;

	/*!
	*	Normalizes this vector
	*/
	void Normalize();

	/*!
	*	\return whether this vector has been normalized
	*/
	bool IsNormalized() const;

	/*!
	*	\param o the other vector
	*	\return the distance between each vector, squared
	*/
	f32 Distance(const Vector2 &o) const;

	/*!
	*	\param o the other vector
	*	\return the distance between each vector, squared
	*	\note unlike Distance, this does not use the square root to get the actual accurate distance
	*/
	f32 DistanceSquare(const Vector2 &o) const;

	/*!
	*	\param o the other vector
	*	\return the dot product between both vectors
	*/
	f32 Dot(const Vector2 &o) const;

	/*!
	*	\return this vector with floor() applied on it
	*/
	Vector2 Floor() const;

	/*!
	*	\return this vector with ceil() applied on it
	*/
	Vector2 Ceil() const;

	/*!
	*	\return this vector with round() applied on it
	*/
	Vector2 Round() const;

	/*!
	*	\return a string in the form of x, y
	*/
	std::string ToString() const;

	/*!
	*	Interpolates (Linearly) between two vectors
	*	\param From the starting point
	*	\param To the end point
	*	\param t a value between 0 and 1, where 0 will give you From, 1 will give you To, and anything in-between will be an interpolation
	*	\return the interpolated point
	*	\note also known as Lerp
	*/
	static Vector2 LinearInterpolate(const Vector2 &From, const Vector2 &To, const f32 &t);

	/*!
	*	Special case so we can rotate something on the virtual "Z" axis
	*	without converting to Vector4 and transforming with a Matrix4x4
	*
	*	\param Angle is an angle in radians
	*	\param In is the vector we want to rotate
	*/
	static Vector2 Rotate(const Vector2 &In, f32 Angle);
};

/*!
*	3D Vector (Point) class
*	Uses x, y, z
*/
class Vector3
{
public:
	f32 x, y, z;
	Vector3();
	Vector3(f32 x, f32 y, f32 z);
	Vector3(const Vector3 &o);
	Vector3(const Vector2 &v);
	Vector3(const Vector2 &xy, f32 z);
	Vector3(f32 x, const Vector2 &yz);

	Vector3 operator+(const Vector3 &o) const;
	Vector3 operator+(f32 f) const;

	Vector3 operator-(const Vector3 &o) const;
	Vector3 operator-(f32 f) const;

	Vector3 operator*(const Vector3 &o) const;
	Vector3 operator*(f32 f) const;
	Vector3 operator*(const Matrix4x4 &m) const;

	Vector3 operator/(const Vector3 &o) const;
	Vector3 operator/(f32 f) const;

	void operator+=(const Vector3 &o);
	void operator+=(f32 f);

	void operator-=(const Vector3 &o);
	void operator-=(f32 f);

	void operator/=(const Vector3 &o);
	void operator/=(f32 f);

	void operator*=(const Vector3 &o);
	void operator*=(f32 f);

	bool operator==(const Vector3 &o) const;
	bool operator!=(const Vector3 &o) const;

	bool operator<(const Vector3 &o) const;
	bool operator>(const Vector3 &o) const;
	bool operator<=(const Vector3 &o) const;
	bool operator>=(const Vector3 &o) const;

	Vector3 operator-() const;

	/*!
	*	\return the magnitude (length) of this vector
	*/
	f32 Magnitude() const;

	/*!
	*	\return the magnitude (length) of this vector
	*	\note unlike Magnitude, this does not use the square root to get the actual accurate magnitude
	*/
	f32 MagnitudeSquare() const;

	/*!
	*	\return a Normalized copy of this vector
	*/
	Vector3 Normalized() const;

	/*!
	*	Normalizes this vector
	*/
	void Normalize();

	/*!
	*	\return whether this vector has been normalized
	*/
	bool IsNormalized() const;

	/*!
	*	\param o the other vector
	*	\return the distance between each vector, squared
	*/
	f32 Distance(const Vector3 &o) const;

	/*!
	*	\param o the other vector
	*	\return the distance between each vector, squared
	*	\note unlike Distance, this does not use the square root to get the actual accurate distance
	*/
	f32 DistanceSquare(const Vector3 &o) const;

	/*!
	*	\param o the other vector
	*	\return the dot product between both vectors
	*/
	f32 Dot(const Vector3 &o) const;

	/*!
	*	\param o the other vector
	*	\return the cross product between both vectors
	*/
	Vector3 Cross(const Vector3 &o) const;

	/*!
	*	\return a Vector2 version of this vector (drops the Z axis)
	*/
	Vector2 ToVector2() const;

	/*!
	*	\return this vector with floor() applied on it
	*/
	Vector3 Floor() const;

	/*!
	*	\return this vector with ceil() applied on it
	*/
	Vector3 Ceil() const;

	/*!
	*	\return this vector with round() applied on it
	*/
	Vector3 Round() const;

	/*!
	*	\return a string in the form of x, y, z
	*/
	std::string ToString() const;

	/*!
	*	\param v1 the first vertice
	*	\param v2 the second vertice
	*	\param v3 the third vertice
	*	\return the normal of a triangle composed of v1, v2, and v3
	*/
	static Vector3 NormalTri(const Vector3 &v1, const Vector3 &v2,
		const Vector3 &v3);

	/*!
	*	\param Vector the vector to use
	*	\return the angles of this vector's components
	*/
	static Vector3 VectorAngles(const Vector3 &Vector);

	/*!
	*	\param Angles the angles to use
	*	\return the original vector of the angles contained in the Angles vector
	*/
	static Vector3 AnglesVector(const Vector3 &Angles);

	/*!
	*	Interpolates (Linearly) between two vectors
	*	\param From the starting point
	*	\param To the end point
	*	\param t a value between 0 and 1, where 0 will give you From, 1 will give you To, and anything in-between will be an interpolation
	*	\return the interpolated point
	*	\note also known as Lerp
	*/
	static Vector3 LinearInterpolate(const Vector3 &From, const Vector3 &To, const f32 &t);
};

/*!
*	4D Vector (Point) class
*	Uses x, y, z, w
*/
class Vector4
{
public:
	f32 x, y, z, w;
	Vector4();
	Vector4(f32 x, f32 y, f32 z);
	Vector4(f32 x, f32 y, f32 z, f32 w);
	Vector4(const Vector4 &o);
	Vector4(const Vector2 &v);
	Vector4(const Vector2 &xy, f32 z, f32 w);
	Vector4(f32 x, const Vector2 &yz);
	Vector4(f32 x, const Vector2 &yz, f32 w);
	Vector4(f32 x, f32 y, const Vector2 &zw);
	Vector4(const Vector2 &xy, const Vector2 &zw);
	Vector4(const Vector3 &v);
	Vector4(const Vector3 &xyz, f32 w);
	Vector4(f32 x, const Vector3 &yzw);

	Vector4 operator+(const Vector4 &o) const;
	Vector4 operator+(f32 f) const;

	Vector4 operator-(const Vector4 &o) const;
	Vector4 operator-(f32 f) const;

	Vector4 operator*(const Vector4 &o) const;
	Vector4 operator*(f32 f) const;
	Vector4 operator*(const Matrix4x4 &m) const;

	Vector4 operator/(const Vector4 &o) const;
	Vector4 operator/(f32 f) const;

	void operator+=(const Vector4 &o);
	void operator+=(f32 f);

	void operator-=(const Vector4 &o);
	void operator-=(f32 f);

	void operator/=(const Vector4 &o);
	void operator/=(f32 f);

	void operator*=(const Vector4 &o);
	void operator*=(f32 f);

	bool operator==(const Vector4 &o) const;
	bool operator!=(const Vector4 &o) const;

	bool operator<(const Vector4 &o) const;
	bool operator>(const Vector4 &o) const;
	bool operator<=(const Vector4 &o) const;
	bool operator>=(const Vector4 &o) const;

	Vector4 operator-() const;

	/*!
	*	\return the magnitude (length) of this vector
	*/
	f32 Magnitude() const;

	/*!
	*	\return the magnitude (length) of this vector
	*	\note unlike Magnitude, this does not use the square root to get the actual accurate magnitude
	*/
	f32 MagnitudeSquare() const;

	/*!
	*	\return a Normalized copy of this vector
	*/
	Vector4 Normalized() const;

	/*!
	*	Normalizes this vector
	*/
	void Normalize();

	/*!
	*	\return whether this vector has been normalized
	*/
	bool IsNormalized() const;

	/*!
	*	\param o the other vector
	*	\return the distance between each vector, squared
	*/
	f32 Distance(const Vector4 &o) const;

	/*!
	*	\param o the other vector
	*	\return the distance between each vector, squared
	*	\note unlike Distance, this does not use the square root to get the actual accurate distance
	*/
	f32 DistanceSquare(const Vector4 &o) const;

	/*!
	*	\param o the other vector
	*	\return the dot product between both vectors
	*/
	f32 Dot(const Vector4 &o) const;

	/*!
	*	\param o the other vector
	*	\return the cross product between both vectors
	*/
	Vector4 Cross(const Vector4 &o) const;

	/*!
	*	\return a Vector2 version of this vector (drops the Z axis and W axis)
	*/
	Vector2 ToVector2() const;

	/*!
	*	\return a Vector3 version of this vector (drops the W axis)
	*/
	Vector3 ToVector3() const;

	/*!
	*	\return this vector with floor() applied on it
	*/
	Vector4 Floor() const;

	/*!
	*	\return this vector with ceil() applied on it
	*/
	Vector4 Ceil() const;

	/*!
	*	\return this vector with round() applied on it
	*/
	Vector4 Round() const;

	/*!
	*	\return a string in the form of x, y, z
	*/
	std::string ToString() const;

	/*!
	*	Interpolates (Linearly) between two vectors
	*	\param From the starting point
	*	\param To the end point
	*	\param t a value between 0 and 1, where 0 will give you From, 1 will give you To, and anything in-between will be an interpolation
	*	\return the interpolated point
	*	\note also known as Lerp
	*/
	static Vector4 LinearInterpolate(const Vector4 &From, const Vector4 &To, const f32 &t);
};

/*!
*	Plane Point Classifications
*/
namespace PlanePointClassifications
{
	enum PlanePointClassifications
	{
		On_Plane = 0, //!<Within Plane
		Back, //!<Behind Plane
		Front, //!<In front of Plane
	};
};

class Plane 
{
public:
	Vector3 Origin;
	f32 Distance;

	Plane();
	Plane(f32 a, f32 b, f32 c, f32 d);
	Plane(const Vector3 &Origin, f32 Distance);
	Plane(const Plane& o);

	/*!
	*	\return a Plane created from a triangle composed of v1, v2, and v3
	*	\param v1 the first vertice
	*	\param v2 the second vertice
	*	\param v3 the third vertice
	*/
	static Plane FromTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
	/*!
	*	\return a classification from PlanePointClassifications
	*	\param v the vector to classify
	*/
	uint32 ClassifyPoint(const Vector3 &v) const;
};

/*!
*	Rectangle class
*/
class Rect 
{
public:
	f32 Left, Right, Top, Bottom;

	Rect();
	Rect(const Rect &Other);
	Rect(f32 Width, f32 Height);
	Rect(f32 Left, f32 Right, f32 Top, f32 Bottom);

	/*!
	*	\return whether Position is inside this rectangle
	*	\param Position a position to check for intersection with this rectangle
	*/
	bool IsInside(const Vector2 &Position) const;

	/*!
	*	\return whether Position is outside this rectangle
	*	\param Position a position to check for intersection with this rectangle
	*/
	bool IsOutside(const Vector2 &Position) const;

	Rect operator+(const Rect &Other) const
	{
		return Rect(Left + Other.Left, Right + Other.Right, Top + Other.Top, Bottom + Other.Bottom);
	};

	Rect operator-(const Rect &Other) const
	{
		return Rect(Left - Other.Left, Right - Other.Right, Top - Other.Top, Bottom - Other.Bottom);
	};

	/*!
	*	\return the size of this Rect
	*/
	Vector2 Size() const
	{
		return Vector2(Right - Left, Bottom - Top);
	};

	/*!
	*	\return the position of this Rect
	*/
	Vector2 Position() const
	{
		return Vector2(Left, Top);
	};
};

/*!
*	Rotateable version of Rect
*/
class RotateableRect
{
public:
	f32 Left, Right, Top, Bottom, Rotation;

	RotateableRect();
	RotateableRect(const RotateableRect &Other);
	RotateableRect(f32 Width, f32 Height, f32 Rotation = 0);
	RotateableRect(f32 Left, f32 Right, f32 Top, f32 Bottom, f32 Rotation = 0);

	/*!
	*	\return whether Position is inside this rectangle
	*	\param Position a position to check for intersection with this rectangle
	*/
	bool IsInside(const Vector2 &Position) const;

	/*!
	*	\return whether Position is outside this rectangle
	*	\param Position a position to check for intersection with this rectangle
	*/
	bool IsOutside(const Vector2 &Position) const;

	RotateableRect operator+(const RotateableRect &Other) const
	{
		return RotateableRect(Left + Other.Left, Right + Other.Right, Top + Other.Top, Bottom + Other.Bottom);
	};

	RotateableRect operator-(const RotateableRect &Other) const
	{
		return RotateableRect(Left - Other.Left, Right - Other.Right, Top - Other.Top, Bottom - Other.Bottom);
	};

	/*!
	*	\return the size of this Rect
	*/
	Vector2 Size() const
	{
		return Vector2(Right - Left, Bottom - Top);
	};

	/*!
	*	\return the position of this Rect
	*/
	Vector2 Position() const
	{
		return Vector2(Left, Top);
	};
	
	/*!
	*	\return the full size of this Rect
	*/
	Vector2 ToFullSize() const
	{
		return Vector2(Right + Left, Bottom + Top);
	};
};

/*!
*	A 3D Box class that is aligned to axis
*/
class AxisAlignedBoundingBox 
{
public:
	Vector3 min, max;
	AxisAlignedBoundingBox();
	AxisAlignedBoundingBox(const AxisAlignedBoundingBox &Other);
	AxisAlignedBoundingBox(const Vector3 &min, const Vector3 &max);

	/*!
	*	Invalidates this AABB
	*/
	void Clear();

	/*!
	*	Calculates this AABB from points
	*	\param Points the points to be used for calculation
	*/
	void Calculate(const std::vector<Vector3> &Points);
	/*!
	*	Calculates this AABB from a point
	*	\param Point the point to be used for calculation
	*/
	void Calculate(const Vector3 &Point);

	/*!
	*	\return whether this AABB intersects another
	*	\param o the other AABB
	*/
	bool Intersects(const AxisAlignedBoundingBox &o) const;

	/*!
	*	\return whether Position is inside this AABB
	*	\param Position a position to check for intersection with this AABB
	*/
	bool IsInside(const Vector3 &Position) const;

	/*!
	*	\return whether Position is outside this AABB
	*	\param Position a position to check for intersection with this AABB
	*/
	bool IsOutside(const Vector3 &Position) const;

	/*!
	*	Gets the coordinates of the 8 corners of this AABB
	*	\param Destination 8 Vector3 objects (passed as a pointer)
	*/
	void Corners(Vector3 *Destination) const;
};

/*!
*	Bounding Sphere class
*/
class BoundingSphere 
{
public:
	Vector3 Center;
	f32 Radius;
	BoundingSphere();
	BoundingSphere(const Vector3 &Center, const f32 &Radius);

	/*!
	*	Calculates this sphere from Points
	*	\param Points the points to calculate this sphere with
	*/
	void Calculate(const std::vector<Vector3> &Points);

	/*!
	*	\return whether this Sphere intersects a Point
	*	\param Point a point to check for intersection with this Sphere
	*/
	bool Intersects(const Vector3 &Point) const;

	/*!
	*	\return whether this Sphere intersects another Sphere
	*	\param Sphere a sphere to check for intersection with this Sphere
	*/
	bool Intersects(const BoundingSphere &Sphere) const;
};

/*!
*	Ray Class
*/
class Ray
{
public:
	Vector3 Position, Direction;
	Ray();
	Ray(const Ray &Other);
	Ray(const Vector3 &Position, const Vector3 &Direction);

	/*!
	*	\return whether this ray intersects a triangle composed by v1, v2, and v3
	*	\param v1 the first vertice
	*	\param v2 the second vertice
	*	\param v3 the third vertice
	*	\param IntersectionPoint [optional] will contain the intersection point
	*	\param t [optional] the intersection multiplier
	*/
	bool IntersectsTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3,
		Vector3 *IntersectionPoint = NULL, f32 *t = NULL) const;

	/*!
	*	\return whether this ray intersects a Plane
	*	\param p the plane to test
	*	\param IntersectionPoint [optional] will contain the intersection point
	*	\param t [optional] the intersection multiplier
	*/
	bool IntersectsPlane(const Plane &p, Vector3 *IntersectionPoint = NULL,
		f32 *t = NULL) const;

	/*!
	*	\return whether this ray hits a box
	*	\param aabb the box to test
	*	\param t0 [optional] the closest or farthest intersection multiplier (can be either)
	*	\param t1 [optional] the closest or farthest intersection multiplier (can be either)
	*	\note Use both t0 and t1 to find the closest or farthest intersection (either one may be the farthest)
	*	or none at all
	*/
	bool IntersectsAligned(const AxisAlignedBoundingBox &aabb, f32 *t0 = NULL, f32 *t1 = NULL) const;

	/*!
	*	\return whether this ray hits a sphere
	*	\param bs the sphee to test
	*	\param t [optional] the intersection multiplier
	*/
	bool IntersectsSphere(const BoundingSphere &bs, f32 *t = NULL) const;
};

/*!
*	Quaternion class
*/
class Quaternion 
{
public:
	f32 x, y, z, w;
	Quaternion();
	Quaternion(f32 x, f32 y, f32 z, f32 w);
	Quaternion operator-() const;
	Quaternion operator*(const f32 &f) const;
	Quaternion operator*(const Quaternion &q) const;
	Quaternion operator+(const Quaternion &q) const;
	Quaternion operator-(const Quaternion &q) const;
	bool operator==(const Quaternion &q) const;
	bool operator!=(const Quaternion &q) const;

	/*!
	*	Normalizes this Quaternion
	*/
	void Normalize();

	/*!
	*	\return a normalized copy of this Quaternion
	*/
	Quaternion Normalized() const;

	/*!
	*	\return whether this Quaternion is normalized
	*/
	bool IsNormalized() const;

	/*!
	*	\return a Matrix4x4 representing the rotation in this Quaternion
	*/
	Matrix4x4 ToMatrix4x4() const;

	/*!
	*	\return an axis-rotated Quaternion
	*	\param v the axis to rotate
	*	\param Angle the rotation angle in Radians
	*	\note v should be in the form of 0 or 1 for the axis you want to rotate. For instance, (0, 1, 0) will rotate on the Y axis
	*/
	static Quaternion FromAxis(const Vector3 &v, f32 Angle);

	/*!
	*	\return a bezier interpolated quaternion
	*	\param A the first quaternion
	*	\param B the second quaternion
	*	\param C the third quaternion
	*	\param D the fourt quaternion
	*	\param t the interpolation multiplier [0, 1]
	*/
	static Quaternion BezierInterpolation(const Quaternion &A, const Quaternion &B,
		const Quaternion &C, const Quaternion &D, f32 t);
};

class MathUtils
{
public:
	static const f64 Pi; //!<Pi Constant
	static const f32 Epsilon; //!<Epsilon Constant

	/*!
	*	Calculates a scale while keeping the aspect ratio
	*	\param TargetSize what size we wish to have
	*	\param CurrentSize our current size before we scale
	*	\return the final Scale that should keep the aspect ratio
	*/
	static Vector2 ScaleKeepingAspectRatio(const Vector2 &TargetSize, const Vector2 &CurrentSize);

	/*!
	*	Clamps a value between two min/max values
	*	\param Value the value we want to Clamp
	*	\param Min the minimum value
	*	\param Max the maximum value
	*/
	static inline f32 Clamp(f32 Value, f32 Min = 0.0f, f32 Max = 1.0f)
	{
		return Value < Min ? Min : Value > Max ? Max : Value;
	};

	/*!
	*	Rounds a value so it reaches the next positive integer based on the decimal value of the Value
	*	E.g., for a value of 0.4, you'll get 0, for a value of 0.5 you'll get 1
	*	\param Value the value to round
	*	\return The rounded value
	*/
	static inline f32 Round(f32 Value)
	{
		return floorf(Value + 0.5f);
	};

	static inline f32 Min(f32 A, f32 B)
	{
		return A > B ? B : A;
	};

	static inline f32 Max(f32 A, f32 B)
	{
		return A > B ? A : B;
	};

	/*!
	*	Converts a degree angle to a radian angle
	*	\param Value the angle to convert
	*	\return the converted angle
	*/
	static inline f32 DegToRad(f32 Value)
	{
		return Value * 0.0174444444444444f;
	};

	/*!
	*	Converts a radian angle to a degree angle
	*	\param Value the angle to convert
	*	\return the converted angle
	*/
	static inline f32 RadToDeg(f32 Value)
	{
		return Value * 57.32484076433121f;
	};

	/*!
	*	Converts a HTML color code to a Vector4
	*	\return a Vector4 with the color
	*	\param Value the HTML color code in the form of #AABBCCDD
	*/
	static inline Vector4 ColorFromHTML(const std::string &Value)
	{
		if(Value.length() == 0)
			return Vector4();

		uint32 StartIndex = 0;

		if(Value[0] == '#')
		{
			StartIndex = 1;
		};

		uint32 ComponentID = 0;
		Vector4 Out(0, 0, 0, 1);

		while(StartIndex + 2 <= Value.length())
		{
			int32 Temp;

			if(1 != sscanf(Value.substr(StartIndex, 2).c_str(), "%x", &Temp))
				break;

			switch(ComponentID)
			{
			case 0:
				Out.x = Temp / 255.f;

				break;

			case 1:
				Out.y = Temp / 255.f;

				break;

			case 2:
				Out.z = Temp / 255.f;

				break;

			case 3:
				Out.w = Temp / 255.f;

				break;
			};

			if(ComponentID == 3)
				break;

			ComponentID++;
			StartIndex += 2;
		};

		return Out;
	};

	/*!
		Calculates the viewport offset and size from two sizes keeping aspect ratio
		\param TargetSize the targeted screen size
		\param CurrentSize the original screen size
		\param OutOffset the resulting Offset between the two sizes
		\param OutSize the resulting Size of the screen
	*/
	static void CalculateViewportKeepingAspectRatio(const Vector2 &TargetSize, const Vector2 &CurrentSize, Vector2 &OutOffset,
		Vector2 &OutSize);

	/*!
		Calculates the viewport scale between two sizes keeping aspect ratio
		\param TargetSize the targeted screen size
		\param CurrentSize the original screen size
		\return the Scale between the two sizes
	*/
	static Vector2 ScaleFromViewport(const Vector2 &TargetSize, const Vector2 &CurrentSize);

	/*!
	*	4-point Bézier Interpolation
	*/
	template<typename PointClass>
	static PointClass BezierInterpolate4(const PointClass &p, const PointClass &p2, const PointClass &p3, const PointClass &p4, f32 t)
	{
		f32 InvertedT = 1 - t;
		f32 Pow2T = powf(t, 2);
		f32 Pow2IT = powf(InvertedT, 2);
		f32 Pow3T = powf(t, 3);
		f32 Pow3IT = powf(InvertedT, 3);

		return p * Pow3IT + p2 * 3 * Pow2IT * t + p3 * 3 * InvertedT * Pow2T + p4 * Pow3T;
	};

	/*!
	*	Perform a linear interpolation
	*	\param a the start point
	*	\param b the end point
	*	\param t the interpolator
	*	\note t should be between 0.0 and 1.0
	*/
	template<typename PointClass>
	static PointClass LinearInterpolate(const PointClass &a, const PointClass &b, f32 t)
	{
		return a + (b - a) * t;
	};

	/*!
	*	Perform a smoothstep interpolation
	*	\param a the start point
	*	\param b the end point
	*	\param t the interpolator
	*	\note t should be between 0.0 and 1.0
	*/
	template<typename PointClass>
	static PointClass SmoothstepInterpolate(const PointClass &a, const PointClass &b, f32 t)
	{
		return LinearInterpolate(a, b, t * t * (3 - 2 * t));
	};

	/*!
	*	Perform a spring interpolation
	*	\param a the start point
	*	\param b the end point
	*	\param t the interpolator
	*	\param Factor the buoyancy factor
	*	\note t should be between 0.0 and 1.0
	*	\note Factor should be between 0.1 and 1.0, but this is optional
	*/
	template<typename PointClass>
	static PointClass SpringInterpolate(const PointClass &a, const PointClass &b, f32 t, f32 Factor)
	{
		return LinearInterpolate(a, b, powf(2, -10 * t) * sinf((t - Factor / 4) * (2 * Pi) / Factor) + 1);
	};

	/*!
	*	Perform a bounce interpolation
	*	\param a the start point
	*	\param b the end point
	*	\param t the interpolator
	*	\note t should be between 0.0 and 1.0
	*/
	template<typename PointClass>
	static PointClass BounceInterpolate(const PointClass &a, const PointClass &b, f32 t)
	{
#define BOUNCE(x) (x * x * 8)

		return LinearInterpolate(a, b, t < 0.3535f ? BOUNCE(t) : (t < 0.7408f ? BOUNCE(t - 0.54719f) + 0.7f : (t < 0.9644f ? BOUNCE(t - 0.8526f) + 0.9f : BOUNCE(t - 1.0435f) + 0.95f)));

#undef BOUNCE
	};

	/*!
	*	Perform an acceleration interpolation
	*	\param a the start point
	*	\param b the end point
	*	\param t the interpolator
	*	\param Factor the acceleration factor
	*	\note t should be between 0.0 and 1.0
	*	\note Factor should be between 0.0 and 1.0
	*/
	template<typename PointClass>
	static PointClass AccelerationInterpolate(const PointClass &a, const PointClass &b, f32 t, f32 Factor)
	{
		return LinearInterpolate(a, b, Factor == 1 ? t * t : powf(t, 2 * Factor));
	};
};
