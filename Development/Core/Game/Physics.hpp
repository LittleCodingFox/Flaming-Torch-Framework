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
		class ContactListener : public b2ContactListener
		{
		public:
			void BeginContact(b2Contact *contact) override;
			void EndContact(b2Contact *contact) override;
		};

		class ContactFilter : public b2ContactFilter
		{
		public:
			bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) override;
		};

		DisposablePointer<b2World> World;
		DisposablePointer<ContactListener> TheContactListener;
		DisposablePointer<ContactFilter> TheContactFilter;

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
