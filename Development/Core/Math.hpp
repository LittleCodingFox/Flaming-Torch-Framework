#pragma once

class Vector3;
class Vector4;

class FLAMING_API Matrix4x4
{
public:
	f32 m[4][4];
	Matrix4x4();
	Matrix4x4(const Matrix4x4 &m);
	Matrix4x4 operator*(const Matrix4x4 &m) const;
	/*
		subscript operator
		You can still access it with two indices by using m.
		Kept for simplicity's sake.
	*/
	f32 &operator[](uint32 index) const;

	f32 Determinant() const;
	void Identity();
	void Transpose();
	bool Inverse();
	static Matrix4x4 Scale(Vector4 Scales);
	//Angles are in radians
	static Matrix4x4 Rotate(Vector3 Angles);
	//For regular 3D positions, v.w must be 1
	static Matrix4x4 Translate(Vector4 v);
	static Matrix4x4 Viewport(f32 x, f32 y, f32 Width, f32 Height);
	/*
		Left Handed Perspective Matrix
		FOV must be in radians
	*/
	static Matrix4x4 PerspectiveMatrixLH(f32 FOV, f32 AspectRatio, f32 zNear, f32 zFar);
	/*
		Right Handed Perspective Matrix
		FOV must be in radians
	*/
	static Matrix4x4 PerspectiveMatrixRH(f32 FOV, f32 AspectRatio, f32 zNear, f32 zFar);

	static Matrix4x4 LookAtMatrixLH(Vector3 Position, Vector3 EyePosition, Vector3 UpVector);
	static Matrix4x4 LookAtMatrixRH(Vector3 Position, Vector3 EyePosition, Vector3 UpVector);
	/*
		Left-Handed Ortho Matrix

		Left is the minimum x-value
		Right is the maximum x-value
		Bottom is the minimum y-value
		Top is the maximum y-value
		zNear is the viewport's zNear value
		zFar is the viewport's zFar value
	*/
	static Matrix4x4 OrthoMatrixLH(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 zNear, f32 zFar);
	/*
		Right-Handed Ortho Matrix

		Left is the minimum x-value
		Right is the maximum x-value
		Bottom is the minimum y-value
		Top is the maximum y-value
		zNear is the viewport's zNear value
		zFar is the viewport's zFar value
	*/
	static Matrix4x4 OrthoMatrixRH(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 zNear, f32 zFar);
};

class FLAMING_API Vector2
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

	f32 Magnitude() const;
	f32 MagnitudeSquare() const;
	Vector2 Normalized() const;
	void Normalize();
	bool IsNormalized() const;
	f32 Distance(const Vector2 &o) const;
	f32 DistanceSquare(const Vector2 &o) const;
	f32 Dot(const Vector2 &o) const;

	//Returns a floorf/ceilf vector
	Vector2 Floor() const;
	//Returns a floorf/ceilf vector
	Vector2 Ceil() const;
	//Returns a rounded vector
	Vector2 Round() const;

	static Vector2 LinearInterpolate(const Vector2 &From, const Vector2 &To, const f32 &t);
	/*
		Special case so we can rotate something on the virtual "Z" axis
		without converting to Vector4 and transforming with a Matrix4x4

		Angle is in radians
	*/
	static Vector2 Rotate(const Vector2 &In, f32 Angle);
};

class FLAMING_API Vector3
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

	f32 Magnitude() const;
	f32 MagnitudeSquare() const;
	Vector3 Normalized() const;
	void Normalize();
	bool IsNormalized() const;
	f32 Distance(const Vector3 &o) const;
	f32 DistanceSquare(const Vector3 &o) const;
	f32 Dot(const Vector3 &o) const;
	Vector3 Cross(const Vector3 &o) const;
	Vector2 ToVector2() const;

	//Returns a floorf/ceilf vector
	Vector3 Floor() const;
	//Returns a floorf/ceilf vector
	Vector3 Ceil() const;
	//Returns a rounded vector
	Vector3 Round() const;

	static Vector3 NormalTri(const Vector3 &v1, const Vector3 &v2,
		const Vector3 &v3);
	static Vector3 VectorAngles(const Vector3 &Vector);
	static Vector3 AnglesVector(const Vector3 &Angles);
	static Vector3 LinearInterpolate(const Vector3 &From, const Vector3 &To, const f32 &t);
};

class FLAMING_API Vector4
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

	f32 Magnitude() const;
	f32 MagnitudeSquare() const;
	Vector4 Normalized() const;
	void Normalize();
	bool IsNormalized() const;
	f32 Distance(const Vector4 &o) const;
	f32 DistanceSquare(const Vector4 &o) const;
	f32 Dot(const Vector4 &o) const;
	Vector4 Cross(const Vector4 &o) const;
	Vector2 ToVector2() const;
	Vector3 ToVector3() const;

	//Returns a floorf/ceilf vector
	Vector4 Floor() const;
	//Returns a floorf/ceilf vector
	Vector4 Ceil() const;
	//Returns a rounded vector
	Vector4 Round() const;

	static Vector4 LinearInterpolate(const Vector4 &From, const Vector4 &To, const f32 &t);
};

namespace PlanePointClassifications
{
	enum
	{
		On_Plane = 0,
		Back,
		Front,
	};
};

class FLAMING_API Plane 
{
public:
	Vector3 Origin;
	f32 Distance;

	Plane();
	Plane(f32 a, f32 b, f32 c, f32 d);
	Plane(const Vector3 &Origin, f32 Distance);
	Plane(const Plane& o);
	static Plane FromTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
	//Returns a PlanePointClassifications::* value
	uint32 ClassifyPoint(const Vector3 &v) const;
	/*
	void Split(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, std::vector<Vector3> &CoplanarFront,
		std::vector<Vector3> &CoplanarBack, std::vector<Vector3> &Front, std::vector<Vector3> &Back);
	*/
};

class FLAMING_API Rect 
{
public:
	f32 Left, Right, Top, Bottom;

	Rect();
	Rect(const Rect &Other);
	Rect(f32 Width, f32 Height);
	Rect(f32 Left, f32 Right, f32 Top, f32 Bottom);

	bool IsInside(const Vector2 &Position) const;
	bool IsOutside(const Vector2 &Position) const;

	Rect operator+(const Rect &Other) const
	{
		return Rect(Left + Other.Left, Right + Other.Right, Top + Other.Top, Bottom + Other.Bottom);
	};

	Rect operator-(const Rect &Other) const
	{
		return Rect(Left - Other.Left, Right - Other.Right, Top - Other.Top, Bottom - Other.Bottom);
	};

	Vector2 Size() const
	{
		return Vector2(Right - Left, Bottom - Top);
	};

	Vector2 Position() const
	{
		return Vector2(Left, Top);
	};
	
	Vector2 ToFullSize() const
	{
		return Vector2(Right + Left, Bottom + Top);
	};
};

class FLAMING_API RotateableRect
{
public:
	f32 Left, Right, Top, Bottom, Rotation;

	RotateableRect();
	RotateableRect(const RotateableRect &Other);
	RotateableRect(f32 Width, f32 Height, f32 Rotation = 0);
	RotateableRect(f32 Left, f32 Right, f32 Top, f32 Bottom, f32 Rotation = 0);

	bool IsInside(const Vector2 &Position) const;
	bool IsOutside(const Vector2 &Position) const;

	RotateableRect operator+(const RotateableRect &Other) const
	{
		return RotateableRect(Left + Other.Left, Right + Other.Right, Top + Other.Top, Bottom + Other.Bottom);
	};

	RotateableRect operator-(const RotateableRect &Other) const
	{
		return RotateableRect(Left - Other.Left, Right - Other.Right, Top - Other.Top, Bottom - Other.Bottom);
	};

	Vector2 Size() const
	{
		return Vector2(Right - Left, Bottom - Top);
	};

	Vector2 Position() const
	{
		return Vector2(Left, Top);
	};
	
	Vector2 ToFullSize() const
	{
		return Vector2(Right + Left, Bottom + Top);
	};
};

class FLAMING_API AxisAlignedBoundingBox 
{
public:
	Vector3 min, max;
	AxisAlignedBoundingBox();
	AxisAlignedBoundingBox(const AxisAlignedBoundingBox &Other);
	AxisAlignedBoundingBox(const Vector3 &min, const Vector3 &max);

	void Clear();
	void Calculate(const std::vector<Vector3> &Points);
	void Calculate(const Vector3 &Point);
	bool Intersects(const AxisAlignedBoundingBox &o) const;
	bool IsInside(const Vector3 &Position) const;
	bool IsOutside(const Vector3 &Position) const;
	//Requires 8 vertices
	void Corners(Vector3 *Destination) const;
};

class FLAMING_API BoundingSphere 
{
public:
	Vector3 Center;
	f32 Radius;
	BoundingSphere();
	BoundingSphere(const Vector3 &Center, const f32 &Radius);
	void Calculate(const std::vector<Vector3> &Points);
	bool Intersects(const Vector3 &Point) const;
	bool Intersects(const BoundingSphere &Sphere) const;
};

class FLAMING_API Ray 
{
public:
	Vector3 Position, Direction;
	Ray();
	Ray(const Ray &Other);
	Ray(const Vector3 &Position, const Vector3 &Direction);
	bool IntersectsTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3,
		Vector3 *IntersectionPoint = NULL, f32 *t = NULL) const;
	bool IntersectsPlane(const Plane &p, Vector3 *IntersectionPoint = NULL,
		f32 *t = NULL) const;
	/*
	*	Use both t0 and t1 to find the closest or farthest intersection (either one may be the farthest)
	*	or none at all
	*/
	bool IntersectsAligned(const AxisAlignedBoundingBox &aabb, f32 *t0 = NULL, f32 *t1 = NULL) const;
	bool IntersectsSphere(const BoundingSphere &bs, f32 *t = NULL) const;
};

class FLAMING_API Quaternion 
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

	void Normalize();
	Quaternion Normalized() const;
	bool IsNormalized() const;
	Matrix4x4 ToMatrix4x4() const;
	bool FromCompressed(const Vector3 &v);
	Vector3 Compress();

	static Quaternion FromAxis(const Vector3 &v, f32 Angle);
	static Quaternion BezierInterpolation(const Quaternion &A, const Quaternion &B,
		const Quaternion &C, const Quaternion &D, f32 t);
};

class FLAMING_API MathUtils
{
public:
	static const f64 Pi;
	static const f32 Epsilon;

	/*!
		Calculates a scale while keeping the aspect ratio
		\param TargetSize what size we wish to have
		\param CurrentSize our current size before we scale
		\return the final Scale that should keep the aspect ratio
	*/
	static Vector2 ScaleKeepingAspectRatio(const Vector2 &TargetSize, const Vector2 &CurrentSize);

	/*!
		Clamps a value between two min/max values
		\param Value the value we want to Clamp
		\param Min the minimum value
		\param Max the maximum value
	*/
	static inline f32 Clamp(f32 Value, f32 Min = 0.0f, f32 Max = 1.0f)
	{
		return Value < Min ? Min : Value > Max ? Max : Value;
	};

	/*!
		Rounds a value so it reaches the next positive integer based on the decimal value of the Value
		E.g., for a value of 0.4, you'll get 0, for a value of 0.5 you'll get 1
		\param Value the value to round
		\return The rounded value
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
		Converts a degree angle to a radian angle
		\param Value the angle to convert
		\return the converted angle
	*/
	static inline f32 DegToRad(f32 Value)
	{
		return Value * 0.0174444444444444f;
	};

	/*!
		Converts a radian angle to a degree angle
		\param Value the angle to convert
		\return the converted angle
	*/
	static inline f32 RadToDeg(f32 Value)
	{
		return Value * 57.32484076433121f;
	};

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
};
