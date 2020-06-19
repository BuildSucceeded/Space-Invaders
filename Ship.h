#pragma once

#include "resource.h"
#include "Point2D.h"

class Ship
{
public:
	Ship();
	Ship(int lifeNo);
	~Ship();

	void Reset();
	void InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget);
	void GoLeft(double elapsedTime);
	void GoRight(double elapsedTime);
	void Advance(double elapsedTime);
	void Kill();
	bool IsHit(Point2D projectile);

	void Draw(ID2D1HwndRenderTarget* m_pRenderTarget);

	Point2D GetPosition();
private:
	Point2D position;
	int lifeStage;
	double explosionTime;

	ID2D1SolidColorBrush* m_pGreenBrush;
	ID2D1SolidColorBrush* m_pRedBrush;
};

