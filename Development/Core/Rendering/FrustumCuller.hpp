#pragma once

/*!
*	Axis-Aligned Bounding Box Test Results
*	\sa AxisAlignedBoundingBox
*/
namespace AABBResults
{
	enum AABBResult
	{
		Visible, //!<The AABB is Visible
		Intersecting, //!<The AABB is Intersecting our Frustum
		Invisible //!<The AABB is Invisible
	};
}

/*!
*	A Frustum Culling class
*/
class FrustumCuller 
{
private:
	Plane Planes[6];
public:
	/*!
	*	Update our Planes with our View and Projection matrices
	*	\param View our View Matrix
	*	\param Projection our Projection Matrix
	*	\sa Matrix4x4
	*	\sa Plane
	*/
	void Update(const Matrix4x4 &View, const Matrix4x4 &Projection);
	/*!
	*	Checks whether a Point is visible
	*	\param v the Point to check
	*	\return Whether the Point is visible
	*/
	bool PointTest(const Vector3 &v);
	/*!
	*	Checks whether a Sphere is visible
	*	\param bs the Sphere to check
	*	\return Whether the Sphere is visible
	*	\sa BoundingSphere
	*/
	bool SphereTest(const BoundingSphere &bs);
	/*!
	*	Checks whether an Axis-Aligned Bounding Box is visible
	*	\param aabb our Axis-Aligned Bounding Box
	*	\return a AABBResults value
	*	\sa AABBResults
	*	\sa AxisAlignedBoundingBox
	*/
	AABBResults::AABBResult AABBTest(const AxisAlignedBoundingBox &aabb);
};
