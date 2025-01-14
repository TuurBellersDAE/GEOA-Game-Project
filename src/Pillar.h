#pragma once
#include "FlyFish.h"
#include "utils.h"

class Pillar
{
public:
    Pillar(Point2f position, float radius);
	~Pillar() = default;
    void Draw() const;
    ThreeBlade GetPosition() const;
	TwoBlade GetLine() const;

private:
    ThreeBlade m_Position;
    float m_Radius;
    Color4f m_Color;
};
