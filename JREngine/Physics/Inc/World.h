#pragma once

namespace Physics
{

struct Settings
{
	Settings()
		: gravity(0.0f,-9.81f,0.0f)
		, timeStep{1.0f/60.0f}
		, drag{0.0f}
		, iterations{1}
	{}

	Math::Vector3 gravity;
	float timeStep;
	float drag;
	uint32_t iterations;
};

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();

	void Setup(const Settings& settings) { mSettings = settings; }

	void Update(float deltaTime);

	void AddParticle(Particle* p);
	void AddConstraint(Constraint* c);
	void AddPhysicsPlane(PhysicsPlane* p);
	void AddPhysicsOBB(PhysicsOBB* obb);
	void AddCube(Physics::PhysicsWorld& world, Math::Vector3 position, Math::Vector3 velocity, float width = 1.0f, float mass = 1.0f, bool fixed = false);
	void ClearDynamic();
	void ClearParticles();

	void DebugDraw() const;

private:
	void AccumulateForces();
	void Integrate();
	void SatisfyConstraints();
	void RemoveExpired();

	Settings mSettings;
	ParticleVec mParticles;
	ConstraintVec mConstraints;
	PhysicsPlaneVec mPlanes;
	PhysicsOBBVec mOBBs;
	float mTimer = 0.0f;
	float mWorldTime = 0.0f;

}; // class PhysicsWorld

} // namespace Physics