#pragma once
#include "utils.h"
#include "structs.h"
#include "FlyFish.h"
#include "Pillar.h"

struct Particle
{
	ThreeBlade position;
	TwoBlade velocity;
	float lifetime;
	Color4f color;
};

class Player
{
public:
	Player(const Point2f& pos);
	~Player();

	void Update(float elapsedSec);
	void Draw() const;

	void Move(float elapsedSec);
	void Rotate(float elapsedSec, const TwoBlade& rotationTarget);
	void Boost(float elapsedSec);
	void SlowDown(float elapsedSec);
	void ToggleBoosting(const bool keyDown);
	void ToggleRotating();
	void HandleCollision();
	void TriggerCollision();
	void Mirror();

	void GenerateParticles(float elapsedSec);
	void DrawParticles() const;

	void LoadRotationTargets(std::vector<Pillar>& pillars);
	void ChangeRotationTarget(Point2f mousePos, float radius, bool isPressed);
	void CalculateTangent();

	void TakeDamage();
	void AddMirrorUses();

	Point2f GetPosition() const;
	Rectf GetBoundaries() const;
	float GetEnergy() const;
	float GetSpeed() const;
	float GetMaxSpeed() const;
	float GetHealth() const;
	float GetDissiness() const;
	int GetMirrorUses() const;
	int GetMirrorUsesMax() const;

	bool IsRotating() const;
	bool IsBoosting() const;

private:
    MultiVector m_Player;

    ThreeBlade m_Position;
    TwoBlade m_DirectionVector;
    OneBlade m_MirrorPlane;
    float& m_Energy;
	float m_EnergyMax;
	float m_EnergyRegen;
	bool m_IsBoostOnCooldown;

	float m_Health;
	float m_Dizziness;
	float m_DizzinessRegen;
	const int m_DizzinessMax;
	int m_MirrorUses;
	int m_MirrorUsesMax;

	TwoBlade m_CurrentRotationTarget;
	std::vector<Pillar> m_RotationTargets;

    float m_Angle;
    float m_Size;

    float m_Speed;
    float m_MaxSpeed;
	float m_MinSpeed;
    float m_BoostAccel;
    float m_Friction;
    float m_RotationSpeed;
    float m_MaxRotationSpeed;

	bool m_IsBoosting;
	bool m_IsRotating;


	std::vector<Particle> m_Particles;
};