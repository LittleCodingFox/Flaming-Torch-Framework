#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "Physics"
#	define PHYSICS_TIMESTEP 1 / 60.f
	const uint64 PHYSICS_TIMESTEP_MILLIS = (uint64)(PHYSICS_TIMESTEP * 1000);

	PhysicsWorld PhysicsWorld::Instance;

	PhysicsWorld::PhysicsWorld() : SubSystem(PHYSICS_PRIORITY), Gravity(0, 9.81f), UpdateTimer(0)
	{
	}

	DisposablePointer<PhysicsBody> PhysicsWorld::MakeBody(bool Dynamic, const Vector2 &Position, const Vector2 &Size, f32 Angle, bool FixedRotation, f32 Density, f32 Friction)
	{
		b2BodyDef BodyDef;
		BodyDef.angle = Angle;
		BodyDef.position = b2Vec2(Position.x, Position.y);
		BodyDef.type = Dynamic ? b2_dynamicBody : b2_staticBody;
		BodyDef.fixedRotation = FixedRotation;

		DisposablePointer<PhysicsBody> Out(new PhysicsBody);

		Out->Body = World->CreateBody(&BodyDef);

		b2PolygonShape BodyShape;
		BodyShape.SetAsBox(Size.x / 2, Size.y / 2);

		b2FixtureDef Fixture;
		Fixture.density = Density;
		Fixture.friction = Friction;
		Fixture.shape = &BodyShape;

		Out->Body->CreateFixture(&Fixture);
		Out->SizeValue = Size;

		Bodies.push_back(Out);

		return Out;
	}

	void PhysicsWorld::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK();

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Initializing Physics...");

		World.Reset(new b2World(b2Vec2(Gravity.x, Gravity.y)));
	}

	void PhysicsWorld::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo(TAG, "Terminating Physics...");

		while (Bodies.size())
		{
			Bodies.begin()->Dispose();
			Bodies.erase(Bodies.begin());
		}

		World.Dispose();
	}

	void PhysicsWorld::Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		if (GameClockDiffNoPause(UpdateTimer) >= PHYSICS_TIMESTEP_MILLIS)
		{
			UpdateTimer = GameClockTimeNoPause();

			World->Step(PHYSICS_TIMESTEP, 8, 3);
		}
	}
}
