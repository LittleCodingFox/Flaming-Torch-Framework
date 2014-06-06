#include "FlamingCore.hpp"
namespace FlamingTorch
{
	void FrustumCuller::Update(const Matrix4x4 &View, const Matrix4x4 &Projection)
	{
		Matrix4x4 Clip = View * Projection;
		Vector4 Vector;
		f32 Magnitude = 0.0f;

		//Plane 0
		Vector = Vector4(Clip.m[0][3] - Clip.m[0][0],
		Clip.m[1][3] - Clip.m[1][0], Clip.m[2][3] - Clip.m[2][0],
		Clip.m[3][3] - Clip.m[3][0]);
		Magnitude = Vector3(Vector.x, Vector.y, Vector.z).Magnitude();

		Planes[0].Origin = Vector3(Vector.x / Magnitude, Vector.y / Magnitude, Vector.z / Magnitude);
		Planes[0].Distance = Vector.w / Magnitude;

		//Plane 1
		Vector = Vector4(Clip.m[0][3] + Clip.m[0][0],
		Clip.m[1][3] + Clip.m[1][0], Clip.m[2][3] + Clip.m[2][0],
		Clip.m[3][3] + Clip.m[3][0]);
		Magnitude = Vector3(Vector.x, Vector.y, Vector.z).Magnitude();

		Planes[1].Origin = Vector3(Vector.x / Magnitude, Vector.y / Magnitude, Vector.z / Magnitude);
		Planes[1].Distance = Vector.w / Magnitude;

		//Plane 2
		Vector = Vector4(Clip.m[0][3] + Clip.m[0][1],
		Clip.m[1][3] + Clip.m[1][1],
		Clip.m[2][3] + Clip.m[2][1],
		Clip.m[3][3] + Clip.m[3][1]);
		Magnitude = Vector3(Vector.x, Vector.y, Vector.z).Magnitude();

		Planes[2].Origin = Vector3(Vector.x / Magnitude, Vector.y / Magnitude, Vector.z / Magnitude);
		Planes[2].Distance = Vector.w / Magnitude;

		//Plane 3
		Vector = Vector4(Clip.m[0][3] - Clip.m[0][1],
		Clip.m[1][3] - Clip.m[1][1], Clip.m[2][3] - Clip.m[2][1],
		Clip.m[3][3] - Clip.m[3][1]);
		Magnitude = Vector3(Vector.x, Vector.y, Vector.z).Magnitude();

		Planes[3].Origin = Vector3(Vector.x / Magnitude, Vector.y / Magnitude, Vector.z / Magnitude);
		Planes[3].Distance = Vector.w / Magnitude;

		//Plane 4
		Vector = Vector4(Clip.m[0][3] - Clip.m[0][2],
		Clip.m[1][3] - Clip.m[1][2], Clip.m[2][3] - Clip.m[2][2],
		Clip.m[3][3] - Clip.m[3][2]);
		Magnitude = Vector3(Vector.x, Vector.y, Vector.z).Magnitude();

		Planes[4].Origin = Vector3(Vector.x / Magnitude, Vector.y / Magnitude, Vector.z / Magnitude);
		Planes[4].Distance = Vector.w / Magnitude;

		//Plane 5
		Vector = Vector4(Clip.m[0][3] + Clip.m[0][2],
		Clip.m[1][3] + Clip.m[1][2], Clip.m[2][3] + Clip.m[2][2],
		Clip.m[3][3] + Clip.m[3][2]);
		Magnitude = Vector3(Vector.x, Vector.y, Vector.z).Magnitude();

		Planes[5].Origin = Vector3(Vector.x / Magnitude, Vector.y / Magnitude, Vector.z / Magnitude);
		Planes[5].Distance = Vector.w / Magnitude;
	};

	bool FrustumCuller::PointTest(const Vector3 &v)
	{
		for(unsigned long i = 0; i < 6; i++)
		{
			if(Planes[i].ClassifyPoint(v) == PlanePointClassifications::Back)
				return false;
		};

		return true;
	};

	bool FrustumCuller::SphereTest(const BoundingSphere &bs)
	{
		for( int i = 0; i < 6; ++i )
		{
			if(Planes[i].Origin.x * bs.Center.x + Planes[i].Origin.y * bs.Center.y + Planes[i].Origin.z * bs.Center.z +
				Planes[i].Distance <= -bs.Radius )
				return false;
		};

		return true;
	};

	AABBResults::AABBResult FrustumCuller::AABBTest(const AxisAlignedBoundingBox &aabb)
	{
		AABBResults::AABBResult Result = AABBResults::Visible;

		for(uint32 i = 0; i < 6; i++)
		{
			Vector4 Positive(
				Planes[i].Origin.x > 0 ? aabb.max.x : aabb.min.x,
				Planes[i].Origin.y > 0 ? aabb.max.y : aabb.min.y,
				Planes[i].Origin.z > 0 ? aabb.max.z : aabb.min.z,
				1.0
				);

			Vector4 Negative(
				Planes[i].Origin.x < 0 ? aabb.max.x : aabb.min.x,
				Planes[i].Origin.y < 0 ? aabb.max.y : aabb.min.y,
				Planes[i].Origin.z < 0 ? aabb.max.z : aabb.min.z,
				1.0
				);

			f32 t = Positive.Dot(*(Vector4*)&Planes[i]);

			if(t < 0)
				return AABBResults::Invisible;

			t = Negative.Dot(*(Vector4*)&Planes[i]);

			if(t < 0)
				Result = AABBResults::Intersecting;
		};

		return Result;
	};
};
