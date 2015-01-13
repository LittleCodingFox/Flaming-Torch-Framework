#pragma once
}
#include "Box2D/Box2D.h"
namespace FlamingTorch
{
	class PhysicsBody
	{
		friend class PhysicsWorld;
	private:
		b2Body *Body;
		Vector2 SizeValue;
	public:
		Vector2 Position() const;
		f32 Rotation() const;
		Vector2 Size() const;
		void SetPosition(const Vector2 &Position);
		void SetRotation(f32 Rotation);
	};

	class PhysicsWorld : public SubSystem
	{
	private:
		DisposablePointer<b2World> World;

		uint64 UpdateTimer;

		std::vector<DisposablePointer<PhysicsBody> > Bodies;

		void StartUp(uint32 Priority);
		void Shutdown(uint32 Priority);
		void Update(uint32 Priority);
	public:
		static PhysicsWorld Instance;

		Vector2 Gravity;

		PhysicsWorld();
		DisposablePointer<PhysicsBody> MakeBody(bool Dynamic, const Vector2 &Position, const Vector2 &Size, f32 Angle, bool FixedRotation, f32 Density, f32 Friction);
	};
