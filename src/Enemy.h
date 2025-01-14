#pragma once
#include "utils.h"
#include "Player.h"

class Enemy
{
public:
    Enemy(const Point2f& pos);
    ~Enemy();

    void Update(float elapsedSec,Player& player);
    void Draw() const;

    Point2f GetPosition() const;
    Rectf GetBoundaries() const;

    bool IsDead() const { return m_IsDead; }
    void Revive(const Point2f& pos);

private:
    void Move(float elapsedSec);
    void HandleCollision(Player& player);

    ThreeBlade m_Position;
    TwoBlade m_DirectionVector;
    float m_Speed;
    float m_MaxSpeed;
    float m_MinSpeed;
    float m_Friction;
    float m_Size;

    bool m_IsDead;
};

