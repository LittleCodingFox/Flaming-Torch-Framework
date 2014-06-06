#include "FlamingCore.hpp"

namespace FlamingTorch
{
	void Camera::SetOrtho(Rect Bounds, f32 zNear, f32 zFar)
	{
		ProjectionTransform = Matrix4x4::OrthoMatrixRH(Bounds.Left, Bounds.Right, Bounds.Top, Bounds.Bottom, zNear, zFar);
	};

	void Camera::SetPerspective(f32 FOV, f32 Aspect, f32 zNear, f32 zFar)
	{
		ProjectionTransform = Matrix4x4::PerspectiveMatrixRH(FOV, Aspect, zNear, zFar);
	};

#if USE_GRAPHICS
	void Camera::BeginTransforms()
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadMatrixf(&ProjectionTransform.m[0][0]);

		Matrix4x4 CameraTransform = WorldTransform;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		CameraTransform.Inverse();

		CameraTransform = CameraTransform * Matrix4x4::Translate(Vector4(DrawOffset, 1));
		glLoadMatrixf(&CameraTransform.m[0][0]);
	};

	void Camera::EndTransforms()
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	};
#endif
};
