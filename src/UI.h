#pragma once
#include "Player.h"
#include "utils.h"

class UI
{
public:
	UI(const Player& player);
	void Update(float elapsedSec);
	void Draw() const;

private:
	const Player& m_Player;
	float m_EnergyBarWidth;
	float m_BarHeight;
	float m_BarPadding;
	Color4f m_EnergyBarColor;
	Color4f m_DizzinessBarColor;
	Color4f m_MirrorBarColor;
	Color4f m_BackgroundColor;
	float m_CircleRadius; // New member variable for circle radius
};
