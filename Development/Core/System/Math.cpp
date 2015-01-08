#include "FlamingCore.hpp"
namespace FlamingTorch
{
	Vector2::Vector2() : x(0), y(0) {}
	Vector2::Vector2(f32 X, f32 Y) : x(X), y(Y) {}
	Vector2::Vector2(const Vector2 &o) : x(o.x), y(o.y) {}

	Vector2 Vector2::operator+(const Vector2 &o) const
	{
		return Vector2(x + o.x, y + o.y);
	}

	Vector2 Vector2::operator+(f32 f) const
	{
		return Vector2(x + f, y + f);
	}

	Vector2 Vector2::operator-(const Vector2 &o) const
	{
		return Vector2(x - o.x, y - o.y);
	}

	Vector2 Vector2::operator-(f32 f) const
	{
		return Vector2(x - f, y - f);
	}

	Vector2 Vector2::operator*(const Vector2 &o) const
	{
		return Vector2(x * o.x, y * o.y);
	}

	Vector2 Vector2::operator*(f32 f) const
	{
		return Vector2(x * f, y * f);
	}

	Vector2 Vector2::operator/(const Vector2 &o) const
	{
		return Vector2(x / ((o.x > 0) ? (o.x) : (1)), y / ((o.y > 0) ? (o.y) : (1)));
	}

	Vector2 Vector2::operator/(f32 f) const
	{
		f32 Final = f != 0 ? f : 1;

		return Vector2(x / Final, y / Final);
	}

	void Vector2::operator+=(const Vector2 &o)
	{
		*this = *this + o;
	}

	void Vector2::operator+=(f32 f)
	{
		*this = *this + f;
	}

	void Vector2::operator-=(const Vector2 &o)
	{
		*this = *this - o;
	}

	void Vector2::operator-=(f32 f)
	{
		*this = *this - f;
	}

	void Vector2::operator/=(const Vector2 &o)
	{
		*this = *this / o;
	}

	void Vector2::operator/=(f32 f)
	{
		*this = *this / f;
	}

	void Vector2::operator*=(const Vector2 &o)
	{
		*this = *this * o;
	}

	void Vector2::operator*=(f32 f)
	{
		*this = *this * f;
	}

	Vector2 Vector2::operator-() const
	{
		return Vector2(-x, -y);
	}

	bool Vector2::operator==(const Vector2 &o) const
	{
		return x == o.x && y == o.y;
	}

	bool Vector2::operator!=(const Vector2 &o) const
	{
		return x != o.x || y != o.y;
	}

	bool Vector2::operator<(const Vector2 &o) const
	{
		return x < o.x || y < o.y;
	}

	bool Vector2::operator>(const Vector2 &o) const
	{
        return x > o.x || y > o.y;
	}

	bool Vector2::operator<=(const Vector2 &o) const
	{
		return operator<(o) || operator==(o);
	}

	bool Vector2::operator>=(const Vector2 &o) const
	{
		return operator>(o) || operator==(o);
	}

	f32 Vector2::Magnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	f32 Vector2::MagnitudeSquare() const
	{
		return x * x + y * y;
	}

	Vector2 Vector2::Normalized() const
	{
		Vector2 v = *this;
		f32 Mag = Magnitude();

		if(Mag <= 0) Mag = 1;

		v.x /= Mag;
		v.y /= Mag;

		return v;
	}

	void Vector2::Normalize()
	{
		f32 Mag = Magnitude();

		if(Mag <= 0) Mag = 1;

		x /= Mag;
		y /= Mag;
	}

	bool Vector2::IsNormalized() const
	{
		return (x == 0 && y == 0) || MagnitudeSquare() == 1;
	}

	f32 Vector2::Distance(const Vector2 &o) const
	{
		return Vector2(o.x - x, o.y - y).Magnitude();
	}

	f32 Vector2::DistanceSquare(const Vector2 &o) const
	{
		return Vector2(o.x - x, o.y - y).MagnitudeSquare();
	}

	f32 Vector2::Dot(const Vector2 &o) const
	{
		return x * o.x + y * o.y;
	}

	Vector2 Vector2::Floor() const
	{
		return Vector2(floorf(x), floorf(y));
	}

	Vector2 Vector2::Ceil() const
	{
		return Vector2(ceilf(x), ceilf(y));
	}

	Vector2 Vector2::Round() const
	{
		return Vector2(MathUtils::Round(x), MathUtils::Round(y));
	}

	std::string Vector2::ToString() const
	{
		return StringUtils::MakeFloatString(x) + ", " + StringUtils::MakeFloatString(y);
	}

	Vector2 Vector2::LinearInterpolate(const Vector2 &From, const Vector2 &To, const f32 &t)
	{
		return From + (To - From) * t;
	}

	Vector2 Vector2::Rotate(const Vector2 &In, f32 Angle)
	{
		f32 cosA(cosf(Angle)), sinA(sinf(Angle));

		return Vector2(In.x * cosA - In.y * sinA, In.y * cosA + In.x * sinA);
	}

	Vector3::Vector3() : x(0), y(0), z(0) {}

	Vector3::Vector3(f32 X, f32 Y, f32 Z) : x(X), y(Y), z(Z) {}

	Vector3::Vector3(const Vector3 &o) : x(o.x), y(o.y), z(o.z) {}

	Vector3::Vector3(const Vector2 &v) : x(v.x), y(v.y), z(0) {}

	Vector3::Vector3(const Vector2 &xy, f32 Z) : x(xy.x), y(xy.y), z(Z) {}

	Vector3::Vector3(f32 X, const Vector2 &yz) : x(X), y(yz.x), z(yz.y) {}

	Vector3 Vector3::operator+(const Vector3 &o) const
	{
		return Vector3(x + o.x, y + o.y, z + o.z);
	}

	Vector3 Vector3::operator+(f32 f) const
	{
		return Vector3(x + f, y + f, z + f);
	}

	Vector3 Vector3::operator-(const Vector3 &o) const
	{
		return Vector3(x - o.x, y - o.y, z -  o.z);
	}

	Vector3 Vector3::operator-(f32 f) const
	{
		return Vector3(x - f, y - f, z - f);
	}

	Vector3 Vector3::operator*(const Vector3 &o) const
	{
		return Vector3(x * o.x, y * o.y, z * o.z);
	}

	Vector3 Vector3::operator*(f32 f) const
	{
		return Vector3(x * f, y * f, z * f);
	}

	Vector3 Vector3::operator*(const Matrix4x4 &m) const
	{
		Vector4 v = Vector4(x, y, z, 1) * m;

		return Vector3(v.x, v.y, v.z);
	}

	Vector3 Vector3::operator/(const Vector3 &o) const
	{
		return Vector3(x / ((o.x > 0) ? (o.x) : (1)), y / ((o.y > 0) ? (o.y) : (1)), z / ((o.z > 0) ? (o.z) : (1)));
	}

	Vector3 Vector3::operator/(f32 f) const
	{
		f32 Final = f != 0 ? f : 1;

		return Vector3(x / Final, y / Final, z / Final);
	}

	void Vector3::operator+=(const Vector3 &o)
	{
		*this = *this + o;
	}

	void Vector3::operator+=(f32 f)
	{
		*this = *this + f;
	}

	void Vector3::operator-=(const Vector3 &o)
	{
		*this = *this - o;
	}

	void Vector3::operator-=(f32 f)
	{
		*this = *this - f;
	}

	void Vector3::operator/=(const Vector3 &o)
	{
		*this = *this / o;
	}

	void Vector3::operator/=(f32 f)
	{
		*this = *this / f;
	}

	void Vector3::operator*=(const Vector3 &o)
	{
		*this = *this * o;
	}

	void Vector3::operator*=(f32 f)
	{
		*this = *this * f;
	}

	Vector3 Vector3::operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	bool Vector3::operator==(const Vector3 &o) const
	{
		return x == o.x && y == o.y && z == o.z;
	}

	bool Vector3::operator!=(const Vector3 &o) const
	{
		return x != o.x || y != o.y || z == o.z;
	}

	bool Vector3::operator<(const Vector3 &o) const
	{
		return x < o.x || y < o.y || z < o.z;
	}

	bool Vector3::operator>(const Vector3 &o) const
	{
		return x > o.x || y > o.y || z > o.z;
	}

	bool Vector3::operator<=(const Vector3 &o) const
	{
		return operator<(o) || operator==(o);
	}

	bool Vector3::operator>=(const Vector3 &o) const
	{
		return operator>(o) || operator==(o);
	}

	f32 Vector3::Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	f32 Vector3::MagnitudeSquare() const
	{
		return x * x + y * y + z * z;
	}

	Vector3 Vector3::Normalized() const
	{
		Vector3 v = *this;
		f32 Mag = Magnitude();
		
		if(Mag <= 0) Mag = 1;

		v.x /= Mag;
		v.y /= Mag;
		v.z /= Mag;

		return v;
	}

	void Vector3::Normalize()
	{
		f32 Mag = Magnitude();
		
		if(Mag <= 0) Mag = 1;

		x /= Mag;
		y /= Mag;
		z /= Mag;
	}

	bool Vector3::IsNormalized() const
	{
		return (x == 0 && y == 0 && z == 0) || MagnitudeSquare() == 1;
	}

	f32 Vector3::Distance(const Vector3 &o) const
	{
		return Vector3(o.x - x, o.y - y, o.z - z).Magnitude();
	}

	f32 Vector3::DistanceSquare(const Vector3 &o) const
	{
		return Vector3(o.x - x, o.y - y, o.z - z).MagnitudeSquare();
	}

	f32 Vector3::Dot(const Vector3 &o) const
	{
		return x * o.x + y * o.y + z * o.z;
	}

	Vector3 Vector3::Cross(const Vector3 &o) const
	{
		return Vector3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
	}

	Vector2 Vector3::ToVector2() const
	{
		return Vector2(x, y);
	}

	Vector3 Vector3::Floor() const
	{
		return Vector3(floorf(x), floorf(y), floorf(z));
	}

	Vector3 Vector3::Ceil() const
	{
		return Vector3(ceilf(x), ceilf(y), ceilf(z));
	}

	Vector3 Vector3::Round() const
	{
		return Vector3(MathUtils::Round(x), MathUtils::Round(y), MathUtils::Round(z));
	}

	std::string Vector3::ToString() const
	{
		return StringUtils::MakeFloatString(x) + ", " + StringUtils::MakeFloatString(y) + ", " + StringUtils::MakeFloatString(z);
	}

	Vector3 Vector3::NormalTri(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
	{
		return (v3 - v1).Cross(v2 - v1).Normalized();
	}

	Vector3 Vector3::VectorAngles(const Vector3 &Vector)
	{
		f32 Magnitude = Vector.Magnitude();

		return Vector3(Magnitude, acosf(Vector.z / Magnitude), atan2f(Vector.y, Vector.x));
	}

	Vector3 Vector3::AnglesVector(const Vector3 &Angles)
	{
		f32 siny(sinf(Angles.y));

		return Vector3(Angles.x * siny * cosf(Angles.z), Angles.x * siny * sinf(Angles.z),
			Angles.x * cosf(Angles.y));
	}

	Vector3 Vector3::LinearInterpolate(const Vector3 &From, const Vector3 &To, const f32 &t)
	{
		return From + (To - From) * t;
	}

	Vector4::Vector4() : x(0), y(0), z(0), w(0) {}
	Vector4::Vector4(f32 X, f32 Y, f32 Z) : x(X), y(Y), z(Z), w(1) {}
	Vector4::Vector4(f32 X, f32 Y, f32 Z, f32 W) : x(X), y(Y), z(Z), w(W) {}
	Vector4::Vector4(const Vector4 &o) : x(o.x), y(o.y), z(o.z), w(o.w) {}
	Vector4::Vector4(const Vector2 &xy, f32 Z, f32 W) : x(xy.x), y(xy.y), z(Z), w(W) {}
	Vector4::Vector4(f32 X, const Vector2 &yz) : x(X), y(yz.x), z(yz.y), w(0) {}
	Vector4::Vector4(f32 X, const Vector2 &yz, f32 W) : x(X), y(yz.x), z(yz.y), w(W) {}
	Vector4::Vector4(f32 X, f32 Y, const Vector2 &zw) : x(X), y(Y), z(zw.x), w(zw.y) {}
	Vector4::Vector4(const Vector2 &xy, const Vector2 &zw) : x(xy.x), y(xy.y), z(zw.x),
		w(zw.y) {}
	Vector4::Vector4(const Vector3 &xyz, f32 W) : x(xyz.x), y(xyz.y), z(xyz.z), w(W) {}
	Vector4::Vector4(f32 X, const Vector3 &yzw) : x(X), y(yzw.x), z(yzw.y), w(yzw.z) {}
	Vector4::Vector4(const Vector2 &v) : x(v.x), y(v.y), z(0), w(0) {}
	Vector4::Vector4(const Vector3 &v) : x(v.x), y(v.y), z(v.z), w(0) {}

	Vector4 Vector4::operator+(const Vector4 &o) const
	{
		return Vector4(x + o.x, y + o.y, z + o.z, w + o.w);
	}

	Vector4 Vector4::operator+(f32 f) const
	{
		return Vector4(x + f, y + f, z + f, w + f);
	}

	Vector4 Vector4::operator-(const Vector4 &o) const
	{
		return Vector4(x - o.x, y - o.y, z - o.z, w - o.w);
	}

	Vector4 Vector4::operator-(f32 f) const
	{
		return Vector4(x - f, y - f, z - f, w - f);
	}

	Vector4 Vector4::operator*(const Vector4 &o) const
	{
		return Vector4(x * o.x, y * o.y, z * o.z, w * o.w);
	}

	Vector4 Vector4::operator*(f32 f) const
	{
		return Vector4(x * f, y * f, z * f, w * f);
	}

	Vector4 Vector4::operator*(const Matrix4x4 &m) const
	{
		Vector4 v;
		v.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0];
		v.y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1];
		v.z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2];
		v.w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3];

		return v;
	}

	Vector4 Vector4::operator/(const Vector4 &o) const
	{
		return Vector4(x / ((o.x > 0) ? (o.x) : (1)), y / ((o.y > 0) ? (o.y) : (1)),
			z / ((o.z > 0) ? (o.z) : (1)), w / ((o.w > 0) ? (o.w) : (1)));
	}

	Vector4 Vector4::operator/(f32 f) const
	{
		f32 Final = f != 0 ? f : 1;

		return Vector4(x / Final, y / Final, z / Final, w / Final);
	}

	void Vector4::operator+=(const Vector4 &o)
	{
		*this = *this + o;
	}

	void Vector4::operator+=(f32 f)
	{
		*this = *this + f;
	}

	void Vector4::operator-=(const Vector4 &o)
	{
		*this = *this - o;
	}

	void Vector4::operator-=(f32 f)
	{
		*this = *this - f;
	}

	void Vector4::operator/=(const Vector4 &o)
	{
		*this = *this / o;
	}

	void Vector4::operator/=(f32 f)
	{
		*this = *this / f;
	}

	void Vector4::operator*=(const Vector4 &o)
	{
		*this = *this * o;
	}

	void Vector4::operator*=(f32 f)
	{
		*this = *this * f;
	}

	Vector4 Vector4::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	bool Vector4::operator==(const Vector4 &o) const
	{
		return x == o.x && y == o.y && z == o.z && w == o.w;
	}

	bool Vector4::operator!=(const Vector4 &o) const
	{
		return x != o.x || y != o.y || z != o.z || w != o.w;
	}

	bool Vector4::operator<(const Vector4 &o) const
	{
		return x < o.x || y < o.y || z < o.z || w < o.w;
	}

	bool Vector4::operator>(const Vector4 &o) const
	{
        return x > o.x || y > o.y || z > o.z || w > o.w;
	}

	bool Vector4::operator<=(const Vector4 &o) const
	{
		return operator<(o) || operator==(o);
	}

	bool Vector4::operator>=(const Vector4 &o) const
	{
		return operator>(o) || operator==(o);
	}

	f32 Vector4::Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	f32 Vector4::MagnitudeSquare() const
	{
		return x * x + y * y + z * z + w * w;
	}

	Vector4 Vector4::Normalized() const
	{
		Vector4 v(*this);
		f32 Mag = Magnitude();
		
		if(Mag <= 0) Mag = 1;

		v.x /= Mag;
		v.y /= Mag;
		v.z /= Mag;
		v.w /= Mag;

		return v;
	}

	void Vector4::Normalize()
	{
		f32 Mag = Magnitude();

		if(Mag <= 0) Mag = 1;

		x /= Mag;
		y /= Mag;
		z /= Mag;
		w /= Mag;
	}

	bool Vector4::IsNormalized() const
	{
		return (x == 0 && y == 0 && z == 0 && w == 0) || MagnitudeSquare() == 1;
	}

	f32 Vector4::Distance(const Vector4 &o) const
	{
		return Vector4(o.x - x, o.y - y, o.z - z, o.w - w).Magnitude();
	}

	f32 Vector4::DistanceSquare(const Vector4 &o) const
	{
		return Vector4(o.x - x, o.y - y, o.z - z, o.w - w).MagnitudeSquare();
	}

	f32 Vector4::Dot(const Vector4 &o) const
	{
		return x * o.x + y * o.y + z * o.z + w * o.w;
	}

	Vector4 Vector4::Floor() const
	{
		return Vector4(floorf(x), floorf(y), floorf(z), floorf(w));
	}

	Vector4 Vector4::Ceil() const
	{
		return Vector4(ceilf(x), ceilf(y), ceilf(z), ceilf(w));
	}

	Vector4 Vector4::Round() const
	{
		return Vector4(MathUtils::Round(x), MathUtils::Round(y), MathUtils::Round(z), MathUtils::Round(w));
	}

	Vector4 Vector4::Cross(const Vector4 &o) const
	{
		//TODO: W component
		return Vector4(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x, 0);
	}

	std::string Vector4::ToString() const
	{
		return StringUtils::MakeFloatString(x) + ", " + StringUtils::MakeFloatString(y) + ", " + StringUtils::MakeFloatString(z) + ", " + StringUtils::MakeFloatString(w);
	}

	Vector4 Vector4::LinearInterpolate(const Vector4 &From, const Vector4 &To, const f32 &t)
	{
		return From + (To - From) * t;
	}

	Vector2 Vector4::ToVector2() const
	{
		return Vector2(x, y);
	}

	Vector3 Vector4::ToVector3() const
	{
		return Vector3(x, y, z);
	}

	Plane::Plane() : Distance(0) {}
	Plane::Plane(const Plane &o) : Origin(o.Origin), Distance(o.Distance) {}
	Plane::Plane(f32 A, f32 B, f32 C, f32 D) : Origin(A, B, C), Distance(D) {}
	Plane::Plane(const Vector3 &_Origin, f32 _Distance) : Origin(_Origin), Distance(_Distance) {}

	Plane Plane::FromTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
	{
		Vector3 Normal = (v2 - v1).Cross(v3 - v1).Normalized();

		return Plane(Normal, -Normal.Dot(v1));
	}

	uint32 Plane::ClassifyPoint(const Vector3 &v) const
	{
		f32 Dot = v.Dot(Origin) + Distance;

		if(Dot > MathUtils::Epsilon)
		{
			return PlanePointClassifications::Front;
		}
		else if(Dot < -MathUtils::Epsilon)
		{
			return PlanePointClassifications::Back;
		}

		return PlanePointClassifications::On_Plane;
	}

	Rect::Rect()
	{
		Left = Right = Top = Bottom = 0.0f;
	}

	Rect::Rect(const Rect &Other)
	{
		this->Left = Other.Left;
		this->Right = Other.Right;
		this->Top = Other.Top;
		this->Bottom = Other.Bottom;
	}

	Rect::Rect(f32 Width, f32 Height)
	{
		Left = -Width;
		Right = -Left;
		Top = -Height;
		Bottom = -Top;
	}

	Rect::Rect(f32 Left, f32 Right, f32 Top, f32 Bottom)
	{
		this->Left = Left;
		this->Right = Right;
		this->Top = Top;
		this->Bottom = Bottom;
	}

	bool Rect::IsInside(const Vector2 &Position) const
	{
		return(Position.x >= Left && Position.x <= Right &&
			Position.y >= Top && Position.y <= Bottom);
	}

	bool Rect::IsOutside(const Vector2 &Position) const
	{
		return !IsInside(Position);
	}

	RotateableRect::RotateableRect()
	{
		Left = Right = Top = Bottom = Rotation = 0.0f;
	}

	RotateableRect::RotateableRect(const RotateableRect &Other)
	{
		Left = Other.Left;
		Right = Other.Right;
		Top = Other.Top;
		Bottom = Other.Bottom;
		Rotation = Other.Rotation;
	}

	RotateableRect::RotateableRect(f32 Width, f32 Height, f32 Rotation)
	{
		Left = -Width;
		Right = -Left;
		Top = -Height;
		Bottom = -Top;
		this->Rotation = Rotation;
	}

	RotateableRect::RotateableRect(f32 Left, f32 Right, f32 Top, f32 Bottom, f32 Rotation)
	{
		this->Left = Left;
		this->Right = Right;
		this->Top = Top;
		this->Bottom = Bottom;
		this->Rotation = Rotation;
	}

	bool RotateableRect::IsInside(const Vector2 &Position) const
	{
		if(Rotation == 0)
		{
			return Position.x >= Left && Position.x <= Right && Position.y >= Top && Position.y <= Bottom;
		}

		Vector2 Size = Vector2(Right - Left, Bottom - Top);
		Vector2 Delta = Position - this->Position();
		f32 Distance = sqrtf(Delta.Dot(Delta));
		f32 Angle = atan2f(Delta.y, Delta.x) - Rotation;
		Vector2 NewPosition(cosf(Angle) * Distance, sinf(Angle) * Distance);

		return NewPosition.x > -0.5 * Size.x && NewPosition.x < 0.5 * Size.x && NewPosition.y > -0.5 * Size.y && NewPosition.y < 0.5 * Size.y;
	}

	bool RotateableRect::IsOutside(const Vector2 &Position) const
	{
		return !IsInside(Position);
	}

	AxisAlignedBoundingBox::AxisAlignedBoundingBox()
	{
		Clear();
	}

	AxisAlignedBoundingBox::AxisAlignedBoundingBox(const AxisAlignedBoundingBox &Other)
	{
		this->min = Other.min;
		this->max = Other.max;
	}

	AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Vector3 &min, const Vector3 &max)
	{
		this->min = min;
		this->max = max;
	}

	void AxisAlignedBoundingBox::Clear()
	{
		min = Vector3(9999999.0f, 9999999.0f, 9999999.0f);
		max = -min;
	}

	void AxisAlignedBoundingBox::Calculate(const std::vector<Vector3> &Points)
	{
		for(uint32 i = 0; i < Points.size(); i++)
		{
			const Vector3& v = Points[i];

			if(v.x < min.x)
			{
				min.x = v.x;
			}

			if(v.y < min.y)
			{
				min.y = v.y;
			}

			if(v.z < min.z)
			{
				min.z = v.z;
			}

			if(v.x > max.x)
			{
				max.x = v.x;
			}

			if(v.y > max.y)
			{
				max.y = v.y;
			}

			if(v.z > max.z)
			{
				max.z = v.z;
			}
		}
	}

	void AxisAlignedBoundingBox::Calculate(const Vector3 &Point)
	{
		if(Point.x < min.x)
		{
			min.x = Point.x;
		}

		if(Point.y < min.y)
		{
			min.y = Point.y;
		}

		if(Point.z < min.z)
		{
			min.z = Point.z;
		}

		if(Point.x > max.x)
		{
			max.x = Point.x;
		}

		if(Point.y > max.y)
		{
			max.y = Point.y;
		}

		if(Point.z > max.z)
		{
			max.z = Point.z;
		}
	}

	bool AxisAlignedBoundingBox::Intersects(const AxisAlignedBoundingBox &o) const
	{
		return min.x <= o.max.x && min.y <= o.max.y && min.z <= o.max.z &&
			max.x >= o.min.x && max.y >= o.min.y && max.z >= o.min.z;
	}

	bool AxisAlignedBoundingBox::IsInside(const Vector3 &Position) const
	{
		return Position.x >= min.x && Position.x <= max.x &&
			Position.y >= min.y && Position.y <= max.y &&
			Position.z >= min.z && Position.z <= max.z;
	}

	bool AxisAlignedBoundingBox::IsOutside(const Vector3 &Position) const
	{
		return !IsInside(Position);
	}

	void AxisAlignedBoundingBox::Corners(Vector3 *Destination) const
	{
		Destination[0] = min;
		Destination[1] = Vector3(max.x, min.y, min.z);
		Destination[2] = Vector3(max.x, max.y, min.z);	
		Destination[3] = Vector3(min.x, max.y, min.z);
		Destination[4] = Vector3(min.x, min.y, max.z);
		Destination[5] = Vector3(max.x, min.y, max.z);
		Destination[6] = max;
		Destination[7] = Vector3(min.x, max.y, max.z);
	}

	BoundingSphere::BoundingSphere() : Radius(0)
	{
	}

	BoundingSphere::BoundingSphere(const Vector3 &_Center, const f32 &_Radius) : Center(_Center), Radius(_Radius)
	{
	}

	void BoundingSphere::Calculate(const std::vector<Vector3> &Points)
	{
		SVOIDFLASSERT(Points.size());

		for(uint32 i = 0; i < Points.size(); i++)
		{
			Center += Points[i];
		}

		Center /= (f32)Points.size();

		for(uint32 i = 0; i < Points.size(); i++)
		{
			f32 Distance = (Points[i].Distance(Center));

			if(Distance > Radius)
				Radius = Distance;
		}
	}

	bool BoundingSphere::Intersects(const Vector3 &Point) const
	{
		return Center.Distance(Point) > Radius;
	}

	bool BoundingSphere::Intersects(const BoundingSphere &Sphere) const
	{
		f32 AddedRadius = Radius + Sphere.Radius;
		return Center.Distance(Sphere.Center) > AddedRadius;
	}

	Ray::Ray()
	{
	}

	Ray::Ray(const Ray &Other)
	{
		this->Position = Other.Position;
		this->Direction = Other.Direction;
	}

	Ray::Ray(const Vector3 &Position, const Vector3 &Direction)
	{
		this->Position = Position;
		this->Direction = Direction;
	}

	bool Ray::IntersectsTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3,
		Vector3 *IntersectionPoint, f32 *t) const
	{
		Vector3 Edge1 = v2 - v1, Edge2 = v3 - v1;
		Vector3 pvec = Direction.Cross(Edge2);
		f32 det = Edge1.Dot(pvec);

		if(det > -MathUtils::Epsilon && det < MathUtils::Epsilon)
			return false;

		f32 inv_det = 1.0f / det;
		Vector3 tvec = Position - v1;
		f32 u = tvec.Dot(pvec) * inv_det;

		if(u < 0.0 || u > 1.0)
			return false;

		Vector3 qvec = tvec.Cross(Edge1);
		f32 v = Direction.Dot(qvec) * inv_det;

		if(v < 0.0 || u + v > 1.0)
			return false;

		f32 _t = Edge2.Dot(qvec) * inv_det;

		if(t)
		{
			*t = _t;
		}

		if(IntersectionPoint)
		{
			*IntersectionPoint = Position + (Direction * _t);
		}

		return true;
	}

	bool Ray::IntersectsPlane(const Plane &p, Vector3 *IntersectionPoint, f32 *t) const
	{
		Vector3 Normal = p.Origin;

		if(Normal.Dot(Direction) == 0)
			return false;

		f32 _t = -( Normal.Dot(Position) + p.Distance ) / ( Normal.Dot(Direction) );

		if(t)
			*t = _t;

		if(IntersectionPoint)
			*IntersectionPoint = Position + Direction * _t;

		return true;
	}

	bool RaySlabIntersect(f32 start, f32 dir, f32 min, f32 max, f32& tfirst, f32& tlast)
	{
		if (fabs(dir) < 1.0E-8)
			return (start < max && start > min);

		f32 tmin = (min - start) / dir;
		f32 tmax = (max - start) / dir;

		if (tmin > tmax)
		{
			f32 t = tmax;
			tmax = tmin;
			tmin = t;
		}

		if (tmax < tfirst || tmin > tlast)
			return false;

		if (tmin > tfirst)
			tfirst = tmin;
		if (tmax < tlast)
			tlast  = tmax;

		return true;
	}

	bool Ray::IntersectsAligned(const AxisAlignedBoundingBox &AxisAlignedBoundingBox, f32 *t0, f32 *t1) const
	{
		f32 tfirst = -999999;
		f32 tlast = 999999;

		if (!RaySlabIntersect(Position.x, Direction.x, AxisAlignedBoundingBox.min.x, AxisAlignedBoundingBox.max.x, tfirst, tlast)) return false;
		if (!RaySlabIntersect(Position.y, Direction.y, AxisAlignedBoundingBox.min.y, AxisAlignedBoundingBox.max.y, tfirst, tlast)) return false;
		if (!RaySlabIntersect(Position.z, Direction.z, AxisAlignedBoundingBox.min.z, AxisAlignedBoundingBox.max.z, tfirst, tlast)) return false;

		if(t0)
			*t0 = tfirst;

		if(t1)
			*t1 = tlast;

		return true;
	}

	bool Ray::IntersectsSphere(const BoundingSphere &bs, f32 *t) const
	{
		float A = Direction.Dot(Direction);
		float B = 2 * Direction.Dot(Position);
		float C = Position.Dot(Position) - (bs.Radius * bs.Radius);

		float Discriminant = B * B - 4 * A * C;

		if (Discriminant < 0)
			return false;

		float DiscriminantSqrt = sqrtf(Discriminant);
		float Q;

		if (B < 0)
			Q = (-B - DiscriminantSqrt) / 2.0f;
		else
			Q = (-B + DiscriminantSqrt) / 2.0f;

		float t0 = Q / A;
		float t1 = C / Q;

		if (t0 > t1)
		{
			float temp = t0;
			t0 = t1;
			t1 = temp;
		}

		if (t1 < 0)
			return false;

		if (t0 < 0)
		{
			if(t)
				*t = t1;

			return true;
		}

		if(t)
			*t = t0;

		return true;
	}

	Matrix4x4::Matrix4x4()
	{
		Identity();
	}

	Matrix4x4::Matrix4x4(const Matrix4x4 &m)
	{
		memcpy(this, &m, sizeof(m));
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &o) const
	{
		Matrix4x4 M;

		M.m[0][0] = m[0][0] * o.m[0][0] + m[0][1] * o.m[1][0] + m[0][2] * o.m[2][0] + m[0][3] * o.m[3][0];
		M.m[0][1] = m[0][0] * o.m[0][1] + m[0][1] * o.m[1][1] + m[0][2] * o.m[2][1] + m[0][3] * o.m[3][1];
		M.m[0][2] = m[0][0] * o.m[0][2] + m[0][1] * o.m[1][2] + m[0][2] * o.m[2][2] + m[0][3] * o.m[3][2];
		M.m[0][3] = m[0][0] * o.m[0][3] + m[0][1] * o.m[1][3] + m[0][2] * o.m[2][3] + m[0][3] * o.m[3][3];

		M.m[1][0] = m[1][0] * o.m[0][0] + m[1][1] * o.m[1][0] + m[1][2] * o.m[2][0] + m[1][3] * o.m[3][0];
		M.m[1][1] = m[1][0] * o.m[0][1] + m[1][1] * o.m[1][1] + m[1][2] * o.m[2][1] + m[1][3] * o.m[3][1];
		M.m[1][2] = m[1][0] * o.m[0][2] + m[1][1] * o.m[1][2] + m[1][2] * o.m[2][2] + m[1][3] * o.m[3][2];
		M.m[1][3] = m[1][0] * o.m[0][3] + m[1][1] * o.m[1][3] + m[1][2] * o.m[2][3] + m[1][3] * o.m[3][3];

		M.m[2][0] = m[2][0] * o.m[0][0] + m[2][1] * o.m[1][0] + m[2][2] * o.m[2][0] + m[2][3] * o.m[3][0];
		M.m[2][1] = m[2][0] * o.m[0][1] + m[2][1] * o.m[1][1] + m[2][2] * o.m[2][1] + m[2][3] * o.m[3][1];
		M.m[2][2] = m[2][0] * o.m[0][2] + m[2][1] * o.m[1][2] + m[2][2] * o.m[2][2] + m[2][3] * o.m[3][2];
		M.m[2][3] = m[2][0] * o.m[0][3] + m[2][1] * o.m[1][3] + m[2][2] * o.m[2][3] + m[2][3] * o.m[3][3];

		M.m[3][0] = m[3][0] * o.m[0][0] + m[3][1] * o.m[1][0] + m[3][2] * o.m[2][0] + m[3][3] * o.m[3][0];
		M.m[3][1] = m[3][0] * o.m[0][1] + m[3][1] * o.m[1][1] + m[3][2] * o.m[2][1] + m[3][3] * o.m[3][1];
		M.m[3][2] = m[3][0] * o.m[0][2] + m[3][1] * o.m[1][2] + m[3][2] * o.m[2][2] + m[3][3] * o.m[3][2];
		M.m[3][3] = m[3][0] * o.m[0][3] + m[3][1] * o.m[1][3] + m[3][2] * o.m[2][3] + m[3][3] * o.m[3][3];

		return M;
	}

	bool Matrix4x4::operator==(const Matrix4x4 &o) const
	{
		//Ugly and possibly slow... Any way to improve this?
		return memcmp(this, &o, sizeof(Matrix4x4)) == 0;
	}

	bool Matrix4x4::operator!=(const Matrix4x4 &o) const
	{
		//Ugly and possibly slow... Any way to improve this?
		return memcmp(this, &o, sizeof(Matrix4x4)) != 0;
	}

	void Matrix4x4::Identity()
	{
		static const f32 IdentityData[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		memcpy(this, IdentityData, sizeof(IdentityData));
	}

	void Matrix4x4::Transpose()
	{
		Matrix4x4 m;
		
		for(unsigned int i = 0; i < 4; i++)
		{
			for(unsigned int j = 0; j < 4; j++)
			{
				m.m[i][j] = this->m[j][i];
			}
		}

		*this = m;
	}

	f32 mat3Determinant(f32 m[3][3])
	{
		return m[0][0] * m[1][1] * m[2][2] + m[1][0] * m[2][1] * m[0][2] + 
			m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[1][1] * m[0][2] - 
			m[0][0] * m[2][1] * m[1][2] - m[1][0] * m[0][1] * m[2][2];
	}

	void SplitMatrix4x4ToMat3(const Matrix4x4 &m, uint32 x, uint32 y, f32 *Data)
	{
		if(x > 3 || y > 3)
			return;

		uint32 Curr = 0;

		for(uint32 i = 0; i < 4; i++)
		{
			for(uint32 j = 0; j < 4; j++)
			{
				if(i == x || j == y)
					continue;

				Data[Curr++] = m.m[i][j];
			}
		}
	}

	f32 Matrix4x4::Determinant() const
	{
		//4 pieces for the determinant
		f32 Pieces[4];

		//split by line i and column 0
		for(uint32 i = 0; i < 4; i++)
		{
			f32 RealPiece[3][3];
			SplitMatrix4x4ToMat3(*this, i, 0, &RealPiece[0][0]);

			//calculate determinant for a 3x3 matrix
			Pieces[i] = m[i][0] * mat3Determinant(RealPiece);
		}

		return Pieces[0] - Pieces[1] + Pieces[2] - Pieces[3];
	}

	bool Matrix4x4::Inverse()
	{
		//Calculate determinant, if 0, impossible
		f32 determinant = Determinant();

		if(determinant == 0.0f)
			return false;

		//copy matrix
		Matrix4x4 m;

		for(uint32 i = 0; i < 4; i++)
		{
			for(uint32 j = 0; j < 4; j++)
			{
				//remove row j and column i
				f32 RealPiece[3][3];
				SplitMatrix4x4ToMat3(*this, i, j, &RealPiece[0][0]);

				//calculate determinant and set to copy matrix
				m.m[j][i] = mat3Determinant(RealPiece);
			}
		}

		//Whether we should swap sign
		bool Swap = false;

		//Inverse determinant
		f32 InvDeterminant = 1.0f / determinant;

		for(uint32 i = 0; i < 4; i++)
		{
			for(uint32 j = 0; j < 4; j++)
			{
				//Swap sign if needed, also multiply with Inverse Determinant
				if(Swap)
					m.m[i][j] *= -InvDeterminant;
				else
					m.m[i][j] *= InvDeterminant;

				Swap = !Swap;
			}

			//Swap after each line too
			Swap = !Swap;
		}

		//reset ourselves as the copy matrix
		*this = m;

		return true;
	}

	Matrix4x4 Matrix4x4::Viewport(f32 x, f32 y, f32 Width, f32 Height)
	{
		Matrix4x4 m;

		m.m[0][0] = Width / 2.f;
		m.m[1][1] = -Height / 2.f;
		m.m[0][3] = x + Width / 2.f;
		m.m[1][3] = y + Height / 2.f;

		return m;
	}

	Matrix4x4 Matrix4x4::Translate(Vector4 v)
	{
		Matrix4x4 m;

		m.m[3][0] = v.x;
		m.m[3][1] = v.y;
		m.m[3][2] = v.z;
		m.m[3][3] = v.w;

		return m;
	}

	Matrix4x4 Matrix4x4::Scale(Vector4 Scales)
	{
		Matrix4x4 m;

		m.m[0][0] = Scales.x;
		m.m[1][1] = Scales.y;
		m.m[2][2] = Scales.z;
		m.m[3][3] = Scales.w;

		return m;
	}

	Matrix4x4 Matrix4x4::Rotate(Vector3 Angles)
	{
		Matrix4x4 m;
		f32 sinval = 0.0f, cosval = 0.0f;

		if(Angles.x != 0.0f)
		{
			Matrix4x4 m2;

			cosval = cosf(Angles.x);
			sinval = sinf(Angles.x);

			m2.m[0][0] = 1.0f;
			m2.m[1][1] = cosval;
			m2.m[1][2] = -sinval;
			m2.m[2][1] = sinval;
			m2.m[2][2] = cosval;

			m = m2;
		}

		if(Angles.y != 0.0f)
		{
			Matrix4x4 m2;

			cosval = cosf(Angles.y);
			sinval = sinf(Angles.y);

			m2.m[0][0] = cosval;
			m2.m[0][2] = sinval;
			m2.m[1][1] = 1.0f;
			m2.m[2][0] = -sinval;
			m2.m[2][2] = cosval;

			m = m * m2;
		}

		if(Angles.z != 0.0f)
		{
			Matrix4x4 m2;

			cosval = cosf(Angles.z);
			sinval = sinf(Angles.z);

			m2.m[0][0] = cosval;
			m2.m[0][1] = -sinval;
			m2.m[1][0] = sinval;
			m2.m[1][1] = cosval;
			m2.m[2][2] = 1.0f;

			m = m * m2;
		}

		return m;
	}

	Matrix4x4 Matrix4x4::PerspectiveMatrixLH(f32 FOV, f32 AspectRatio, f32 zNear, f32 zFar)
	{
		f32 cotan = 0.0f, DeltaZ = f32(zFar) - f32(zNear),
			angle = FOV / 2.0f, sine = sinf(angle);

		FLASSERT(sine != 0.0f && DeltaZ != 0.0f && AspectRatio != 0.0f, 
			"Invalid Viewport Information");

		cotan = cosf(angle) / sine;

		Matrix4x4 m;

		m.m[0][0] = cotan / AspectRatio;
		m.m[1][1] = cotan;
		m.m[2][2] = zFar / DeltaZ;
		m.m[2][3] = 1.0f;
		m.m[3][2] = -zNear * zFar / DeltaZ;
		m.m[3][3] = 0.0f;

		return m;
	}

	Matrix4x4 Matrix4x4::PerspectiveMatrixRH(f32 FOV, f32 AspectRatio, f32 zNear, f32 zFar)
	{
		f32 cotan = 0.0f, DeltaZ = f32(zNear) - f32(zFar),
			angle = FOV / 2.0f, sine = sinf(angle);

		FLASSERT(sine != 0.0f && DeltaZ != 0.0f && AspectRatio != 0.0f, 
			"Invalid Viewport Information");

		cotan = cosf(angle) / sine;

		Matrix4x4 m;

		m.m[0][0] = cotan / AspectRatio;
		m.m[1][1] = cotan;
		m.m[2][2] = zFar / DeltaZ;
		m.m[2][3] = -1.0f;
		m.m[3][2] = zNear * zFar / DeltaZ;
		m.m[3][3] = 0.0f;

		return m;
	}

	Matrix4x4 Matrix4x4::LookAtMatrixLH(Vector3 Position, Vector3 EyePosition, Vector3 UpVector)
	{
		Vector3 ZAxis = Vector3(EyePosition - Position).Normalized(), 
			XAxis = UpVector.Cross(ZAxis).Normalized(), 
			YAxis = ZAxis.Cross(XAxis);

		Matrix4x4 m;

		m.m[0][0] = XAxis.x;
		m.m[0][1] = YAxis.x;
		m.m[0][2] = ZAxis.x;
		m.m[1][0] = XAxis.y;
		m.m[1][1] = YAxis.y;
		m.m[1][2] = ZAxis.y;
		m.m[2][0] = XAxis.z;
		m.m[2][1] = YAxis.z;
		m.m[2][2] = ZAxis.z;
		m.m[3][0] = -XAxis.Dot(Position);
		m.m[3][1] = -YAxis.Dot(Position);
		m.m[3][2] = -ZAxis.Dot(Position);

		return m;
	}

	Matrix4x4 Matrix4x4::LookAtMatrixRH(Vector3 Position, Vector3 EyePosition, Vector3 UpVector)
	{
		Vector3 ZAxis = Vector3(Position - EyePosition).Normalized(), 
			XAxis = UpVector.Cross(ZAxis).Normalized(), 
			YAxis = ZAxis.Cross(XAxis);

		Matrix4x4 m;

		m.m[0][0] = XAxis.x;
		m.m[0][1] = YAxis.x;
		m.m[0][2] = ZAxis.x;
		m.m[1][0] = XAxis.y;
		m.m[1][1] = YAxis.y;
		m.m[1][2] = ZAxis.y;
		m.m[2][0] = XAxis.z;
		m.m[2][1] = YAxis.z;
		m.m[2][2] = ZAxis.z;
		m.m[3][0] = -XAxis.Dot(Position);
		m.m[3][1] = -YAxis.Dot(Position);
		m.m[3][2] = -ZAxis.Dot(Position);

		return m;
	}

	Matrix4x4 Matrix4x4::OrthoMatrixLH(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 zNear, f32 zFar)
	{
		Matrix4x4 m;

		m.m[0][0] = 2.0f / (Right - Left);
		m.m[1][1] = 2.0f / (Bottom - Top);
		m.m[2][2] = 1.0f / (zFar - zNear);
		m.m[3][0] = (Left + Right) / (Left - Right);
		m.m[3][1] = (Top + Bottom) / (Bottom - Top);
		m.m[3][2] = zNear / (zNear - zFar);

		return m;
	}

	Matrix4x4 Matrix4x4::OrthoMatrixRH(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 zNear, f32 zFar)
	{
		Matrix4x4 m;

		m.m[0][0] = 2.0f / (Right - Left);
		m.m[1][1] = 2.0f / (Top - Bottom);
		m.m[2][2] = 1.0f / (zNear - zFar);
		m.m[3][0] = (Left + Right) / (Left - Right);
		m.m[3][1] = (Top + Bottom) / (Bottom - Top);
		m.m[3][2] = zNear / (zNear - zFar);

		return m;
	}

	f32 &Matrix4x4::operator[](uint32 Index) const
	{
		return ((f32*)&m[0][0])[Index];
	}

	Quaternion::Quaternion() : x(0), y(0), z(0), w(1) {}

	Quaternion::Quaternion(f32 x, f32 y, f32 z, f32 w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Quaternion Quaternion::operator*(const f32 &f) const
	{
		return Quaternion(x * f, y * f, z * f, w * f);
	}

	Quaternion Quaternion::operator*(const Quaternion &q) const
	{
		Quaternion Out;

		Out.w = w * q.w - x * q.x - y * q.y - z * q.z;
		Out.x = w * q.x + x * q.w + y * q.z - z * q.y;
		Out.y = w * q.y - x * q.z + y * q.w + z * q.x;
		Out.z = w * q.z + x * q.y - y * q.x + z * q.w;

		return Out;
	}

	Quaternion Quaternion::operator+(const Quaternion &q) const
	{
		return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
	}

	Quaternion Quaternion::operator-(const Quaternion &q) const
	{
		return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
	}

	Quaternion Quaternion::operator-() const
	{
		return Quaternion(-x, -y, -z, -w);
	}

	bool Quaternion::operator==(const Quaternion &q) const
	{
		return x == q.x && y == q.y && z == q.z && w == q.w;
	}

	bool Quaternion::operator!=(const Quaternion &q) const
	{
		return x != q.x || y != q.y || z != q.z || w == q.w;
	}

	Quaternion Quaternion::FromAxis(const Vector3 &v, f32 Angle)
	{
		Angle *= 0.5f;
		Vector3 n = v.Normalized();
		float SinAngle = sinf(Angle);
		Quaternion Out;

		Out.x = (n.x * SinAngle);
		Out.y = (n.y * SinAngle);
		Out.z = (n.z * SinAngle);
		Out.w = cosf(Angle);

		return Out;
	}

	void Quaternion::Normalize()
	{
		f32 Length = Vector4(x, y, z, w).Magnitude();

		if(Length == 0.0f)
			return;

		Length = 1.0f / Length;

		x *= Length;
		y *= Length;
		z *= Length;
		w *= Length;
	}

	Quaternion Quaternion::Normalized() const
	{
		Quaternion q(*this);
		q.Normalize();

		return q;
	}

	bool Quaternion::IsNormalized() const
	{
		return Vector4(x, y, z, w).IsNormalized();
	}

	Quaternion Quaternion::BezierInterpolation(const Quaternion &A, const Quaternion &B, const Quaternion &C, const Quaternion &D, f32 t)
	{
		if(t < 0.0f)
			return A;

		if(t > 1.0f)
			return B;

		f32 invT = 1.0f - t;

		f32 t1 = powf(invT, 3.0f);
		f32 t2 = 3.0f * powf(invT, 2.0f) * t;
		f32 t3 = 3.0f * invT * powf(t, 2.0f);
		f32 t4 = powf(t, 3.0f);

		return A * t1 + B * t2 + C * t3 + D * t4;
	}

	Matrix4x4 Quaternion::ToMatrix4x4() const
	{
		Matrix4x4 m;
		f32 xx = x * x;
		f32 xy = x * y;
		f32 xz = x * z;
		f32 xw = x * w;

		f32 yy = y * y;
		f32 yz = y * z;
		f32 yw = y * w;

		f32 zz = z * z;
		f32 zw = z * w;

		m.m[0][0] = 1.0f - 2.0f * (yy + zz);
		m.m[0][1] = 2.0f * (xy + zw);
		m.m[0][2] = 2.0f * (xz - yw);

		m.m[1][0] = 2.0f * (xy - zw);
		m.m[1][1] = 1.0f - 2.0f * (xx + zz);
		m.m[1][2] = 2.0f * (yz + xw);

		m.m[2][0] = 2.0f * (xz + yw);
		m.m[2][1] = 2.0f * (yz - xw);
		m.m[2][2] = 1.0f - 2.0f * (xx + yy);

		return m;
	}
	
	Vector2 MathUtils::ScaleKeepingAspectRatio(const Vector2 &TargetSize, const Vector2 &CurrentSize)
	{
		Vector2 Out(TargetSize.x, TargetSize.y);

		if(CurrentSize.x > CurrentSize.y)
		{
			Out.y = CurrentSize.y * Out.x / CurrentSize.x;
		}
		else
		{
			Out.x = CurrentSize.x * Out.y / CurrentSize.y;
		}

		return Out;
	}
	
	void MathUtils::CalculateViewportKeepingAspectRatio(const Vector2 &TargetSize, const Vector2 &CurrentSize, Vector2 &OutOffset,
		Vector2 &OutSize)
	{
		f32 AspectRatio = CurrentSize.x / (f32)CurrentSize.y;

		Vector2 ActualSize(TargetSize.x, TargetSize.x / AspectRatio + 0.5f);

		if(ActualSize.y > TargetSize.y)
		{
			ActualSize.y = TargetSize.y;
			ActualSize.x = ActualSize.y * AspectRatio + 0.5f;
		}

		OutOffset = (TargetSize - ActualSize) / 2.f;
		OutSize = ActualSize;
	}

	Vector2 MathUtils::ScaleFromViewport(const Vector2 &TargetSize, const Vector2 &CurrentSize)
	{
		return TargetSize / CurrentSize;
	}

	const f64 MathUtils::Pi = 3.14159265358979323846264338327950288419716939937510;
	const f32 MathUtils::Epsilon = 0.000001f;
}
